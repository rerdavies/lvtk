// Copyright (c) 2023 Robin Davies
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "lvtk_ui/Lv2UI.hpp"
#include "lvtk_ui/Lv2PortView.hpp"
#include "lvtk/LvtkSettingsFile.hpp"
#include "lvtk_ui/Lv2PortViewFactory.hpp"

#include "lvtk/LvtkWindow.hpp"
#include "lvtk/LvtkFlexGridElement.hpp"
#include "lvtk/LvtkScrollContainerElement.hpp"
#include "lv2/ui/ui.h"
#include "lv2/atom/atom.h"
#include "lv2/atom/forge.h"
#include "lv2/atom/util.h"
#include "lv2/core/lv2.h"
#include "lv2/core/lv2_util.h"
#include "lv2/log/log.h"
#include "lv2/midi/midi.h"
#include "lv2/port-groups/port-groups.h"
#include "lv2/urid/urid.h"
#include "lv2/options/options.h"
#include <stdio.h>
#include <stdarg.h>

#include <vector>
#include <string.h>

#include "lvtk/LvtkGroupElement.hpp"

using namespace lvtk::ui;
using namespace lvtk;

void Lv2UI::SetCreateWindowDefaults()
{
    LvtkCreateWindowParameters &params = this->createWindowParameters;
    params.positioning = LvtkWindowPositioning::ChildWindow;
    if (params.positioning == LvtkWindowPositioning::NotSet)
    {
        params.positioning = LvtkWindowPositioning::ChildWindow;
    }
    if (params.size == LvtkSize::Zero)
    {
        params.size = LvtkSize(800, 600);
    }
    if (params.minSize == LvtkSize::Zero)
    {
        params.minSize = LvtkSize(320, 200);
    }
    if (params.maxSize == LvtkSize::Zero)
    {
        params.maxSize = LvtkSize(4096, 4096);
    }

    // Controls name of settings file which contains, window position, and File Dialog parameters (if any).
    // Use the same format as for a GTK Application ID:  "orgname" "-" "plugin_key"
    if (params.settingsKey.length() == 0)
    {
        params.settingsKey = this->pluginInfo->name();
    }

    if (params.x11Windowclass.length() == 0)
    {
        params.x11Windowclass = "com.twoplay.lvtk-plugin"; // Maybe used for settings by Window Managers.
    }
    if (params.gtkApplicationId.length() == 0)
    {
        params.gtkApplicationId = "com.twoplay.lvtk-plugin"; // Connects KDE Plasma and Gnome Destkop to a .desktop file that has the actual icon reference.
    }
    if (params.title.length() == 0)
    {
        params.title = pluginInfo->name();
    }
    if (createWindowParameters.x11WindowName.length() == 0)
    {
        if (pluginInfo->label().length() != 0)
        {
            params.x11WindowName = pluginInfo->label();
        }
        else
        {
            params.x11WindowName = pluginInfo->name();
        }
    }
}
Lv2UI::Lv2UI(std::shared_ptr<Lv2PluginInfo> pluginInfo, LvtkSize defaultWindowSize)
    : Lv2UI(pluginInfo, LvtkCreateWindowParameters())
{
    createWindowParameters.size = defaultWindowSize;
    SetCreateWindowDefaults();
}

Lv2UI::Lv2UI(std::shared_ptr<Lv2PluginInfo> pluginInfo, const LvtkCreateWindowParameters &createWindowParameters)
    : pluginInfo(pluginInfo), createWindowParameters(createWindowParameters),
        icuInstance(lvtk::IcuString::Instance()) // lifetime managment for Icu libraries.
{
    this->createWindowParameters.positioning = LvtkWindowPositioning::ChildWindow;

    SetCreateWindowDefaults();

    settingsFile = std::make_shared<LvtkSettingsFile>();
    settingsFile->Load(createWindowParameters.settingsKey);
    this->createWindowParameters.settingsObject = settingsFile->Root();

    this->bindingSites.resize(pluginInfo->ports().size());
    this->bindingSiteObserverHandles.resize(pluginInfo->ports().size());
    this->currentHostPortValues.resize(pluginInfo->ports().size());

    for (size_t i = 0; i < pluginInfo->ports().size(); ++i)
    {
        auto &port = pluginInfo->ports()[i];
        if (port.is_control_port())
        {
            auto index = port.index();
            if (bindingSites[index] != nullptr)
            {
                std::cout << "Error: Duplicate port index." << std::endl;
            }
            LvtkBindingProperty<double> *pBinding = new LvtkBindingProperty<double>();
            bindingSites[index] = pBinding;
            bindingSites[index]->set(port.default_value());
            this->bindingSiteMap[port.symbol()] = bindingSites[index];

            currentHostPortValues[index] = port.default_value();

            if (port.is_input())
            {
                bindingSiteObserverHandles[index] = pBinding->addObserver([this, index](double value)
                                                                          { this->OnPortValueChanged(index, value); });
            }
        }
    }

    this->Theme(LvtkTheme::Create(true));
    this->portViewFactory = Lv2PortViewFactory::Create();
}
 
Lv2UI::~Lv2UI()
{
    if (cairoWindow)
    {
        cairoWindow->CloseRootWindow();
        cairoWindow = nullptr;
    }
    bindingSiteObserverHandles.resize(0);

    for (size_t i = 0; i < bindingSites.size(); ++i)
    {
        delete bindingSites[i];
    }
    bindingSites.resize(0);
    bindingSiteMap.clear();
}

// LV2 callback handlers.
bool Lv2UI::instantiate(
    const char *plugin_ui_uri,
    const char *plugin_uri,
    const char *bundle_path,
    LV2UI_Write_Function write_function,
    LV2UI_Controller controller,
    LV2UI_Widget *widget,
    const LV2_Feature *const *features)
{

    if (this->createWindowParameters.backgroundColor.A() == 0)
    {
        this->createWindowParameters.backgroundColor = Theme()->paper;
    }
    this->pluginUiUri = plugin_ui_uri;
    this->pluginUri = plugin_uri;
    this->bundlePath = bundle_path;
    this->writeFunction = write_function;
    this->controller = controller;
    this->widget = widget;

    LV2_Options_Option* options = nullptr;
    const char *missing = lv2_features_query(
        features,
        LV2_LOG__log, &this->log, false,
        LV2_URID__map, &this->map, true,
        LV2_URID__unmap, &this->unmap, false,
        LV2_UI__requestValue, &this->requestValue, false,
        LV2_OPTIONS__options,&options,false,
        NULL);
    if (missing)
    {
        LogError("Missing " LV2_URID__map "feature.");
        return false;
    }
    LV2_URID lv2ui_scaleFactor = this->GetUrid(LV2_UI__scaleFactor);
    if (options)
    {
        for (LV2_Options_Option*p = options; p->key != 0; ++p)
        {
            std::string option = this->UridToString(p->key);
//            std::cout << "Option: " << option << std::endl;
            if (p->subject == lv2ui_scaleFactor)
            {
                if (p->type == this->urids.atom__Float)
                {
                    this->scaleFactor = *(float*)(p->value);
                }
            }
        }
    }
    InitUrids();

    for (int i = 0; features[i] != nullptr; ++i)
    {
        std::cout << "Feature: " << features[i]->URI << std::endl;
        if (!strcmp(features[i]->URI, LV2_UI__parent))
        {
            this->parentWindow = features[i]->data;
        }
        else if (!strcmp(features[i]->URI, LV2_UI__resize))
        {
            resize = (LV2UI_Resize *)features[i]->data;
        }
    }
    if (!parentWindow)
    {
        LogError("Parent window not found.");
        return false;
    }

    // Display *XOpenDisplay(nullptr);
    WindowHandle windowHandle{(uint64_t)this->parentWindow};
    cairoWindow = LvtkWindow::Create();

    if (theme)
    {
        cairoWindow->Theme(theme);
    }
    cairoWindow->SetResourceDirectories(
        {(std::filesystem::path(this->BundlePath()) / "resources").string()});
    cairoWindow->CreateWindow(windowHandle, createWindowParameters);

    WindowHandle x11Handle = cairoWindow->Handle();

    *widget = (LV2UI_Widget)(void*)x11Handle.getHandle();
    auto ui = this->Render();
    cairoWindow->GetRootElement()->AddChild(ui);
    if (this->resize)
    {
        resize->ui_resize(resize->handle, (int)std::ceil(createWindowParameters.size.Width()), (int)std::ceil(createWindowParameters.size.Height()));
    }
    return true;
}

void Lv2UI::InitUrids()
{
    urids.log__Error = GetUrid(LV2_LOG__Error);
    urids.log__Note = GetUrid(LV2_LOG__Note);
    urids.log__Trace = GetUrid(LV2_LOG__Trace);
    urids.log__Warning = GetUrid(LV2_LOG__Warning);
    urids.atom__Float = GetUrid(LV2_ATOM__Float);
}

void Lv2UI::ui_port_event(
    uint32_t port_index,
    uint32_t buffer_size,
    uint32_t format,
    const void *buffer)
{
    if (format == 0) // port notification.
    {
        float value = *(float *)buffer;
        if (port_index >= 0 && port_index < bindingSites.size())
        {
            this->currentHostPortValues[port_index] = value;
            auto bindingSite = bindingSites[port_index];
            if (bindingSite)
            {
                bindingSite->set(value);
            }
        }
    }
}
int Lv2UI::ui_show()
{

    return 0;
}
int Lv2UI::ui_hide()
{
    return 0;
}
int Lv2UI::ui_idle()
{
    if (cairoWindow)
    {
        cairoWindow->PumpMessages(false);
    }
    return 0;
}
void Lv2UI::ui_delete()
{
    if (cairoWindow)
    {
        cairoWindow->CloseRootWindow();
    }
    cairoWindow = nullptr;
    delete this;
}

const std::string &Lv2UI::PluginUiUri() const { return pluginUiUri; }
const std::string &Lv2UI::PluginUri() const { return pluginUri; }
const std::string &Lv2UI::BundlePath() const { return bundlePath; }

void Lv2UI::VLogError(const char *format, ...)
{
    if (this->log)
    {
        va_list args;
        va_start(args, format);
        log->vprintf(log->handle, urids.log__Error, format, args);
        va_end(args);
    }
    else
    {
        printf("Error: ");
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
        printf("\n");
    }
}
void Lv2UI::LogError(const char *message)
{
    if (this->log)
    {
        log->printf(log->handle, urids.log__Error, "%s", message);
    }
    else
    {
        printf("Error: ");
        printf("%s", message);
        printf("\n");
    }
}

void Lv2UI::LogError(const std::string &message)
{
    LogError(message.c_str());
}

void Lv2UI::VLogNote(const char *format, ...)
{
    if (this->log)
    {
        va_list args;
        va_start(args, format);
        log->vprintf(log->handle, urids.log__Note, format, args);
        va_end(args);
    }
    else
    {
        printf("Note: ");
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
        printf("\n");
    }
}
void Lv2UI::LogNote(const char *message)
{
    if (this->log)
    {
        log->printf(log->handle, urids.log__Note, "%s", message);
    }
    else
    {
        printf("Note: ");
        printf("%s", message);
        printf("\n");
    }
}

void Lv2UI::LogNote(const std::string &message)
{
    LogNote(message.c_str());
}
void Lv2UI::VLogTrace(const char *format, ...)
{
    if (this->log)
    {
        va_list args;
        va_start(args, format);
        log->vprintf(log->handle, urids.log__Trace, format, args);
        va_end(args);
    }
    else
    {
        printf("Trace: ");
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
        printf("\n");
    }
}
void Lv2UI::LogTrace(const char *message)
{
    if (this->log)
    {
        log->printf(log->handle, urids.log__Trace, "%s", message);
    }
    else
    {
        printf("Trace: ");
        printf("%s", message);
        printf("\n");
    }
}

void Lv2UI::LogTrace(const std::string &message)
{
    LogTrace(message.c_str());
}
void Lv2UI::VLogWarning(const char *format, ...)
{
    if (this->log)
    {
        va_list args;
        va_start(args, format);
        log->vprintf(log->handle, urids.log__Warning, format, args);
        va_end(args);
    }
    else
    {
        printf("Warning: ");
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
        printf("\n");
    }
}
void Lv2UI::LogWarning(const char *message)
{
    if (this->log)
    {
        log->printf(log->handle, urids.log__Warning, "%s", message);
    }
    else
    {
        printf("Warning: ");
        printf("%s", message);
        printf("\n");
    }
}

void Lv2UI::LogWarning(const std::string &message)
{
    LogWarning(message.c_str());
}

LV2_URID Lv2UI::GetUrid(const char *urid)
{
    return map->map(map->handle, urid);
}

const char *Lv2UI::UridToString(LV2_URID urid)
{
    if (this->unmap)
    {
        return unmap->unmap(unmap->handle, urid);
    }
    return "#not available.";
}

LvtkBindingProperty<double> &Lv2UI::GetControlProperty(const std::string &key)
{
    auto f = bindingSiteMap.find(key);
    if (f == bindingSiteMap.end())
    {
        throw std::invalid_argument("Key not found.");
    }
    return *(f->second);
}
const LvtkBindingProperty<double> &Lv2UI::GetControlProperty(const std::string &key) const
{
    auto f = bindingSiteMap.find(key);
    if (f == bindingSiteMap.end())
    {
        throw std::invalid_argument("Key not found.");
    }
    return *(f->second);
}

Lv2UI &Lv2UI::SetControlValue(const std::string &key, double value)
{
    GetControlProperty(key).set(value);
    return *this;
}
double Lv2UI::GetControlValue(const std::string &key) const
{
    return GetControlProperty(key).get();
}

LvtkContainerElement::ptr Lv2UI::Render()
{
    LvtkScrollContainerElement::ptr scrollElement = LvtkScrollContainerElement::Create();
    scrollElement->HorizontalScrollEnabled(false)
        .VerticalScrollEnabled(true);
    scrollElement->Style().Background(Theme()->paper)
        .HorizontalAlignment(LvtkAlignment::Stretch)
        .VerticalAlignment(LvtkAlignment::Stretch);
    {
        portViewFactory->Theme(this->theme);
        LvtkContainerElement::ptr container = portViewFactory->CreatePage();

        AddRenderControls(container);
        scrollElement->Child(container);
    }
    return scrollElement;
}

bool Lv2UI::IsVuMeterPair(size_t portIndex)
{
    auto &port = this->pluginInfo->ports()[portIndex];
    // 1) must be an output port.
    if (!port.is_control_port())
        return false;
    if (!port.is_output())
        return false;

    // 2) must have lv2:designation = pg:left.
    if (port.designation() != LV2_PORT_GROUPS__left)
    {
        return false;
    }
    // 3) must have a next port, which must be an output port.
    if (portIndex + 1 >= this->pluginInfo->ports().size())
        return false;
    auto &rightPort = this->pluginInfo->ports()[portIndex + 1];
    if (!port.is_control_port() || !port.is_output())
        return false;
    // 4) and must have designation lv2:designation = pg:right
    if (rightPort.designation() != LV2_PORT_GROUPS__right)
    {
        return false;
    }
    // and must be members of the same portgroup.
    if (port.port_group() != rightPort.port_group())
    {
        return false;
    }
    return true;
}

void Lv2UI::AddRenderControls(LvtkContainerElement::ptr container)
{
    std::map<std::string, LvtkGroupElement::ptr> portGroups;
    for (size_t i = 0; i < this->pluginInfo->ports().size(); ++i)
    {
        auto &port = this->pluginInfo->ports()[i];

        if (port.is_control_port())
        {
            if (port.port_group().length() != 0)
            {

                if (IsVuMeterPair(i))
                {
                    auto &rightPort = this->pluginInfo->ports()[i + 1];
                    std::string label;

                    for (const auto &portGroupInfo : pluginInfo->port_groups())
                    {
                        if (portGroupInfo.uri() == port.port_group())
                        {
                            label = portGroupInfo.name();
                            break;
                        }
                    }

                    container->AddChild(
                        RenderStereoControl(
                            label,
                            GetControlProperty(port.symbol()), port,
                            GetControlProperty(rightPort.symbol()), rightPort));

                    // skip the right control
                    ++i;
                }
                else
                {

                    LvtkGroupElement::ptr portGroup;
                    if (portGroups.contains(port.port_group()))
                    {
                        portGroup = portGroups[port.port_group()];
                    }
                    else
                    {
                        portGroup = LvtkGroupElement::Create();
                        portGroups[port.port_group()] = portGroup;
                        container->AddChild(portGroup);
                        for (const auto &portGroupInfo : pluginInfo->port_groups())
                        {
                            if (portGroupInfo.uri() == port.port_group())
                            {
                                portGroup->Text(portGroupInfo.name());
                                break;
                            }
                        }
                    }
                    portGroup->AddChild(
                        RenderControl(
                            GetControlProperty(port.symbol()),
                            port));
                }
            }
            else
            {
                container->AddChild(
                    RenderControl(
                        GetControlProperty(port.symbol()),
                        port));
            }
        }
    }
}

LvtkElement::ptr Lv2UI::RenderStereoControl(
    const std::string &label,
    LvtkBindingProperty<double> &leftValue, const Lv2PortInfo &leftPortInfo,
    LvtkBindingProperty<double> &rightValue, const Lv2PortInfo &rightPortInfo)
{
    return CreateStereoPortView(label, leftValue, rightValue, leftPortInfo, this->portViewFactory);
}

LvtkElement::ptr Lv2UI::RenderControl(LvtkBindingProperty<double> &value, const Lv2PortInfo &portInfo)
{
    return CreatePortView(value, portInfo, this->portViewFactory);
}

void Lv2UI::OnPortValueChanged(int32_t portIndex, double value)
{
    float floatValue = (float)value;
    if (this->controller != nullptr)
    {
        if (floatValue != this->currentHostPortValues[portIndex])
        {
            this->currentHostPortValues[portIndex] = floatValue;
            this->writeFunction(
                this->controller,
                portIndex,
                sizeof(float),
                0, // 0 => ui:floatProtocol
                &floatValue);
        }
    }
}

LvtkTheme::ptr Lv2UI::Theme()
{
    if (cairoWindow)
    {
        return cairoWindow->ThemePtr();
    }
    return theme;
}

Lv2UI &Lv2UI::Theme(LvtkTheme::ptr theme)
{
    this->theme = theme;
    if (cairoWindow)
    {
        cairoWindow->Theme(theme);
    }
    return *this;
}

Lv2PortViewFactory &Lv2UI::PortViewFactory() const
{
    return *(portViewFactory.get());
}
Lv2UI &Lv2UI::PortViewFactory(std::shared_ptr<Lv2PortViewFactory> value)
{
    this->portViewFactory = value;
    return *this;
}

int Lv2UI::ui_resize(int width, int height)
{
    if (this->cairoWindow)
    {
      //  cairoWindow->Resize(width,height);
    }
    return 0;
}