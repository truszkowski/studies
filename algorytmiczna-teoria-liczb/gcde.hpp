/* 
 * Rozszerzony algorytm euklidesa,
 * Dla x, y znajdujemy:
 *  ax + by = v
 * oraz dodatkowo:
 * inverse() = x^(-1) mod y
 */

#ifndef __GCDE_H__
#define __GCDE_H__

template <class __Type = int>
class gcde {
private:
	__Type __a, __b, __v, __x, __y;
public:
	gcde(void) {}
	gcde(__Type xp, __Type yp) 
	: __x(xp), __y(yp) { }
	gcde(const gcde & g) 
	: __a(g.__a), __b(g.__b), __v(g.__v), 
	__x(g.__x), __y(g.__y) { }
	~gcde(void) { }
	
	const __Type & a() const { return __a; }
	const __Type & b() const { return __b; }
	const __Type & v() const { return __v; }
	const __Type & x() const { return __x; }
	const __Type & y() const { return __y; }

	__Type & x(const __Type & n) { return (__x = n); }
	__Type & y(const __Type & n) { return (__y = n); }
	
	gcde & set(__Type xp, __Type yp) {
		__x = xp; __y = yp;
		return *this;
	}
	
	__Type inverse(void) const { // x(^-1) (mod y)
		return ( __a > 0 ? __a : __y + __a );
// zawsze zwracamy dodatnia wartosc,
	}
	
	gcde & go(void) {
// rozszerzony algorytm Euklidesa,
		__Type g = 1, xp = __x, yp = __y;
		while ((xp % 2 == 0) && (yp % 2 == 0)) {
			xp /= 2; yp /= 2; g /= 2;
		}
		__Type up = xp, vp = yp, A = 1, B = 0, C = 0, D = 1;

		while (up != 0) {
			while ((up % 2) == 0) {
				up /= 2;
				if ((A % 2 == 0) && (B % 2 == 0)) {
					A /= 2; B /= 2;
				} else {
					A = (A + yp) / 2; B = (B - xp) / 2;
				}
			}
			while ((vp % 2) == 0) {
				vp /= 2;
				if ((C % 2 == 0) && (D % 2 == 0)) {
					C /= 2; D /= 2;
				} else {
					C = (C + yp) / 2; D = (D - xp) / 2;
				}
			}
			if (up >= vp) {
				up -= vp; A -= C; B -= D;
			} else {
				vp -= up; C -= A; D -= B;
			}
		}
// end,
		__a = C; __b = D; __v = g*vp;
		return *this;
	}
};

#endif
