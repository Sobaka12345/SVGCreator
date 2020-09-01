#include <iostream>
#include <optional>
#include <variant>
#include <memory>
#include <vector>
#include <string>

using namespace std;

namespace Svg {

struct Point
{
    double x = 0.0;
    double y = 0.0;
};

struct Rgb
{
    ushort red = 0;
    ushort green = 0;
    ushort blue = 0;

    friend ostream& operator<<(ostream& stream, const Rgb& val)
    {
        stream << "rgb(" << val.red << ','
               << val.green << ','
               << val.blue << ")";

        return stream;
    }
};

class Color
{
    optional<variant<string, Rgb>> color_;
public:
    Color() = default;

    Color(string color) :
        color_(move(color))
    {}

    Color(Rgb color) :
        color_(move(color))
    {}

    friend ostream& operator<<(ostream& stream, const Color& val)
    {
        const auto getVal = [&stream](const auto& var) { stream << var; };
        if(val.color_.has_value())
        {
            visit(getVal, val.color_.value());
        } else
            stream << "none";

        return stream;
    }
};

static const Color NoneColor;

class FigureInterface
{
public:
    virtual ~FigureInterface() {};
    virtual void print(ostream&) = 0;
};

template <typename Derived>
class Figure : public FigureInterface
{
protected:
    Color fillColor_, strokeColor_;
    double strokeWidth_;
    optional<string> strokeLineCap_, strokeLineJoin_;

    Figure() :
        fillColor_(NoneColor),
        strokeColor_(NoneColor),
        strokeWidth_(1.0)
    {}

public:

    void print(ostream& stream) override
    {
         stream << *static_cast<Derived*>(this);
    }

    Derived& SetFillColor(const Color& color)
    {
        fillColor_ = color;

        return *static_cast<Derived*>(this);
    }

    Derived& SetFillColor(string color)
    {
        fillColor_ = Color(move(color));

        return *static_cast<Derived*>(this);
    }

    Derived& SetStrokeColor(const Color& color)
    {
        strokeColor_ = color;

        return *static_cast<Derived*>(this);
    }

    Derived& SetStrokeWidth(double width)
    {
        strokeWidth_ = width;

        return *static_cast<Derived*>(this);
    }

    Derived& SetStrokeLineCap(const string& lineCap)
    {
        strokeLineCap_ = lineCap;

        return *static_cast<Derived*>(this);
    }

    Derived& SetStrokeLineJoin(const string lineJoin)
    {
        strokeLineJoin_ = lineJoin;

        return *static_cast<Derived*>(this);
    }
};

class Circle: public Figure<Circle>
{
protected:
    double centerX_, centerY_, radius_;

public:
    Circle() :
        Figure(),
        centerX_(0.0),
        centerY_(0.0)
    {}

    Circle& SetCenter(Point center)
    {
        centerX_ = center.x;
        centerY_ = center.y;

        return *this;
    }

    Circle& SetRadius(double r)
    {
        radius_ = r;

        return *this;
    }

    friend ostream& operator<<(ostream& stream, const Circle& val)
    {
        stream << "<circle ";
        stream << "cx=\"" << val.centerX_ << "\" "
               << "cy=\"" << val.centerY_ << "\" "
               << "r=\"" << val.radius_ << "\" "
               << "fill=\"" << val.fillColor_ << "\" "
               << "stroke=\"" << val.strokeColor_ << "\" "
               << "stroke-width=\"" << val.strokeWidth_ << "\" ";
        if(val.strokeLineCap_.has_value())
            stream << "stroke-linecap=\"" << val.strokeLineCap_.value() << "\" ";
        if(val.strokeLineJoin_.has_value())
            stream << "stroke-linejoin=\"" << val.strokeLineJoin_.value() << "\" ";

        stream << "/>";

        return stream;
    }

};

class Polyline: public Figure<Polyline>
{
protected:
    vector<Point> points_;

public:
    Polyline() :
        Figure()
    {}

    Polyline& AddPoint(Point point)
    {
        points_.push_back(move(point));

        return *this;
    }

    friend ostream& operator<<(ostream& stream, const Polyline& val)
    {
        stream << "<polyline points=\"";
        for(const auto& point : val.points_)
        stream << point.x << "," << point.y << " ";
        stream << "\" "
               << "fill=\"" << val.fillColor_ << "\" "
               << "stroke=\"" << val.strokeColor_ << "\" "
               << "stroke-width=\"" << val.strokeWidth_ << "\" ";
        if(val.strokeLineCap_.has_value())
            stream << "stroke-linecap=\"" << val.strokeLineCap_.value() << "\" ";
        if(val.strokeLineJoin_.has_value())
            stream << "stroke-linejoin=\"" << val.strokeLineJoin_.value() << "\" ";

        stream << "/>";


        return stream;
    }
};

class Text: public Figure<Text>
{
protected:
    Point point_, offset_;
    uint32_t fontSize_;
    optional<string> fontFamily_;
    string data_;

public:
    Text() :
        Figure(),
        fontSize_(1)
    {}

    Text& SetPoint(Point point)
    {
        point_ = move(point);

        return *this;
    }

    Text& SetOffset(Point offset)
    {
        offset_ = move(offset);

        return *this;
    }

    Text& SetFontSize(uint32_t size)
    {
        fontSize_ = size;

        return *this;
    }

    Text& SetFontFamily(const string& family)
    {
        fontFamily_ = family;

        return *this;
    }

    Text& SetData(const string& data)
    {
        data_ = data;

        return *this;
    }

    friend ostream& operator<<(ostream& stream, const Text& val)
    {
        stream << "<text ";
        stream << "x=\"" << val.point_.x << "\" "
               << "y=\"" << val.point_.y << "\" "
               << "dx=\"" << val.offset_.x << "\" "
               << "dy=\"" << val.offset_.y << "\" "
               << "fill=\"" << val.fillColor_ << "\" "
               << "stroke=\"" << val.strokeColor_ << "\" "
               << "font-size=\"" << val.fontSize_ << "\" "
               << "stroke-width=\"" << val.strokeWidth_ << "\" ";
        if(val.fontFamily_.has_value())
            stream << "stroke-linejoin=\"" << val.fontFamily_.value() << "\" ";
        if(val.strokeLineCap_.has_value())
            stream << "stroke-linecap=\"" << val.strokeLineCap_.value() << "\" ";
        if(val.strokeLineJoin_.has_value())
            stream << "stroke-linejoin=\"" << val.strokeLineJoin_.value() << "\" ";
        stream << ">" << val.data_ << "</text>";

        return stream;
    }
};

class Document
{
    vector<unique_ptr<FigureInterface>> figures_;

public:
    Document()
    {}

    template <typename T>
    void Add(T figure)
    {
        figures_.push_back(make_unique<T>(move(figure)));
    }

    void Render(ostream& out)
    {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
            << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">";

        for(const auto& figure: figures_)
            figure->print(out);

        out << "</svg>";
    }
};

}

#include <fstream>

int main()
{
    Svg::Document svg;

    svg.Add(
        Svg::Polyline{}
        .SetStrokeColor(Svg::Rgb{255, 198, 63}) 
        .SetStrokeWidth(16)
        .SetStrokeLineCap("round")
        .AddPoint({50, 50})
        .AddPoint({250, 250})
    );

    for (const auto point : {Svg::Point{50, 50}, Svg::Point{250, 250}}) {
      svg.Add(
          Svg::Circle{}
          .SetFillColor("white")
          .SetRadius(6)
          .SetCenter(point)
      );
    }

    svg.Add(
        Svg::Text{}
        .SetPoint({50, 50})
        .SetOffset({10, -10})
        .SetFontSize(20)
        .SetFontFamily("Verdana")
        .SetFillColor("black")
        .SetData("C")
    );
    svg.Add(
        Svg::Text{}
        .SetPoint({250, 250})
        .SetOffset({10, -10})
        .SetFontSize(20)
        .SetFontFamily("Verdana")
        .SetFillColor("black")
        .SetData("C++")
    );


    svg.Render(cout);

    return 0;
}
