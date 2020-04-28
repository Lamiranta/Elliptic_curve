#include <iostream>
#include <random>
#include <functional>
using namespace std;

struct point
{
	int x, y;
	bool inf = false;
};

/// Weierstrass form of elliptic curve
struct ellcurve
{
    int f(int x)
	    { return (x * x * x) + (a * x) + b; };
    int a, b, p;
};
ellcurve ellcurve;

/// Modulo operator
int modulo(int num)
{
    while (num < 0)
        num += ellcurve.p;
    return num % ellcurve.p;
}

/// Elliptic curve over a finite field
bool** mapping()
{
	bool** field = new bool*[ellcurve.p];
	for (int i = 0; i < ellcurve.p; ++i)
	{
		field[i] = new bool[ellcurve.p];
		for (int j = 0; j < ellcurve.p; ++j)
		{
            field[i][j] = (j * j) % ellcurve.p == modulo(ellcurve.f(i));
		}
	}
	return field;
}

/// Image of finite field
void finite_field (bool** map)
{
    for (int j = ellcurve.p - 1; j >= 0; --j)
    {
        for (int i = 0; i < ellcurve.p; ++i)
        {
            if (map[i][j]) cout << "O ";
            else cout << ". ";
        }
        cout << endl;
    }
}

/// Search of multiplicative inverse
int multinv(int num)
{
    while (num < 0)
        num += ellcurve.p;
    int s = 1;
    while (num * s % ellcurve.p != 1)
        s++;
    return s;
}

/// Key generator
void key_generate(point g, int secret)
{
    point key = g;
    int m = 0;

    for (unsigned int i = 0; i < secret; ++i)
    {
        /// Doubling a point
        if (key.x == g.x && key.y == g.y && !key.inf) {
            m = modulo((3 * (g.x * g.x) + ellcurve.a) * multinv(2 * g.y));
            key.x = m * m - 2 * g.x;
            key.y = m * (g.x - key.x) - g.y;
        }

        /// Point at infinity
        else if (key.x == g.x && !key.inf)
            key.inf = true;
        else if (key.inf) {
            key = g;
            key.inf = false;
        }

        /// Adding points
        else {
            m = modulo((key.y - g.y) * multinv(key.x - g.x));
            key.x = m * m - key.x - g.x;
            key.y = m * (g.x - key.x) - g.y;
        }

        if (!key.inf) {
            key.x = modulo(key.x);
            key.y = modulo(key.y);
            // cout << i + 2 << " * g : (" << key.x << ", " << key.y << ')' << endl;
        }
        // else cout << i + 2 << " * g : (Infinity, Infinity)" << endl;
    }

    if (key.x != -1)
        cout << "Your private key: " << key.x << ' ' << key.y;
    else
        cout << "Your private key: " << g.x << ' ' << g.y;
}

int main()
{
    /// Parameters and prime modulo of elliptic curve
    cout << "Enter elliptic curve and prime modulo:";
    cin >> ellcurve.a >> ellcurve.b >> ellcurve.p;
    system("cls");

    /// Random engine
    default_random_engine rng;
	uniform_int_distribution <int> dist(1, ellcurve.p);
	auto num = bind(dist, rng);

	/// Finite field map
	bool** map = mapping();
    // finite_field(map);

	/// Generate a key
	point g{};
	int secret = num();
	do {
		g.x = num();
		g.y = num();
	} while (!map[g.x][g.y]);
    cout << "g: " << g.x << ' ' << g.y << endl;
	key_generate(g, secret);

	return 0;
}
