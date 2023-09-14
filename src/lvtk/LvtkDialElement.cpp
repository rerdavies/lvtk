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

#include "lvtk/LvtkDialElement.hpp"
#include "lvtk/LvtkWindow.hpp"

using namespace lvtk;

LvtkDialElement::LvtkDialElement()
{
    
    this->dropShadow = LvtkDropShadowElement::Create();
    this->AddChild(dropShadow);
    this->image = LvtkSvgElement::Create();
    dropShadow->AddChild(this->image);
    this->image->Style()
        .HorizontalAlignment(LvtkAlignment::Stretch)
        .VerticalAlignment(LvtkAlignment::Stretch)
        ;

    SourceProperty.Bind(image->SourceProperty);

    OnValueChanged(Value());
}
void LvtkDialElement::OnMount()
{
    super::OnMount();
    this->Classes({this->Theme().dialStyle});
    if (TintImage())
    {
        image->Style().TintColor(Style().Color());
    }
    const LvtkDropShadow &dropShadowValue = 
        this->DropShadow().has_value()
        ? this->DropShadow().value()
        : Theme().dialDropShadow;
    dropShadow->DropShadow(dropShadowValue);
}
void LvtkDialElement::OnValueChanged(double value)
{

    double angle = (this->Value() - 0.5) * (2*135);
    this->image->Rotation(angle);
}

void LvtkDialElement::OnDialOpacityChanged(double opacity) 
{
    this->dropShadow->Style().Opacity(opacity);
    this->dropShadow->Invalidate();
}


