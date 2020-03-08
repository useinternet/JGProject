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
	inline static JColor Red()   { return JColor(1.0f, 0.0f, 0.0f, 1.0f); }
	inline static JColor Green() { return JColor(0.0f, 1.0f, 0.0f, 1.0f); }
	inline static JColor Blue()  { return JColor(0.0f, 0.0f, 1.0f, 1.0f); }
	inline static JColor White() { return JColor(1.0f, 1.0f, 1.0f, 1.0f); }
	inline static JColor Black() { return JColor(0.0f, 0.0f, 0.0f, 1.0f); }
};