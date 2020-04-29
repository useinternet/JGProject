#pragma once



class JColor
{
public:
	float r;
	float g;
	float b;
	float a;
public:
	JColor() : r(0.0f), g(0.0f), b(0.0f), a(0.0f) {}
	JColor(float r, float g, float b, float a) :
		r(r), g(g), b(b), a(a) {}
public:
	void Set(float r, float g, float b, float a) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}
public:
	bool operator==(const JColor& c) {
		return (r == c.r && g == c.g && b == c.b && a == c.a);
	}

public:
	inline static JColor Red()   { return JColor(1.0f, 0.0f, 0.0f, 1.0f); }

	inline static JColor Green() { return JColor(0.0f, 1.0f, 0.0f, 1.0f); }
	inline static JColor Blue()  { return JColor(0.0f, 0.0f, 1.0f, 1.0f); }
	inline static JColor White() { return JColor(1.0f, 1.0f, 1.0f, 1.0f); }
	inline static JColor Black() { return JColor(0.0f, 0.0f, 0.0f, 1.0f); }
	inline static JColor OrangeRed() { return JColor(1.0f, 0.64705f, 0.0f, 1.0f); }
	inline static JColor Orange() { return JColor(1.0f, 0.64705f, 0.0f, 1.0f); }
	inline static JColor DarkOrange() { return JColor(1.0f, 0.270588f, 0.0f, 1.0f); }
	inline static JColor Gold() { return JColor(1.0f, 0.843137f, 0.0f, 1.0f); }
	inline static JColor Yellow() { return JColor(1.0f, 1.0f, 0.0f, 1.0f); }
	//0.64705 //0.270588
};