// Copyright (c) 2023 Robin E. R. Davies
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

#pragma once

#include "Lv2cContainerElement.hpp"

namespace lv2c {

    class Lv2cVerticalStackElement : public Lv2cContainerElement
    {
    public:
        virtual const char* Tag() const override { return "VStack";}

        using super = Lv2cContainerElement;

        using ptr = std::shared_ptr<Lv2cVerticalStackElement>;

        static ptr Create() { return std::make_shared<Lv2cVerticalStackElement>(); }

        Lv2cVerticalStackElement();
        virtual ~Lv2cVerticalStackElement();

    protected:
        virtual bool ClipChildren() const override;
        virtual void Measure(Lv2cSize constraint,Lv2cSize available, Lv2cDrawingContext &context) override;
        virtual Lv2cSize MeasureClient(Lv2cSize constraint,Lv2cSize available, Lv2cDrawingContext &context) override;
        virtual Lv2cSize Arrange(Lv2cSize available,Lv2cDrawingContext &context) override;
    private:
        bool clipChildren = false;
        struct ChildInfo {
            Lv2cSize measureSize;
            Lv2cSize arrangeSize;
        };
        std::vector<ChildInfo> childInfos;
        //StyleContext GetHorizontalMeasureContext(double width, Lv2cElement*element);
    };

}// namespace