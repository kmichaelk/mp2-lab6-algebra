#include <iostream>

#include "polynomial.h"

using namespace std;

int main()
{
    string input;

    cout << "Enter polynomial: ";
    getline(cin, input);

    std::shared_ptr<Polynomial> polynomial;
    try {
        polynomial = std::make_shared<Polynomial>(input);
    } catch (const expression_parse_error& err)
    {
        cerr << "Malformed input: " << err.what() << endl;
        return EXIT_FAILURE;
    }
    polynomial->compact();

    cout << " Parsed: " << polynomial << endl;

    cout << " Derivatives: " << endl;
    for (char var = Monomial::VAR_MIN; var <= Monomial::VAR_MAX; var++) {
        cout << "  '" << var << "': " << polynomial->differentiate(var) << endl;
    }

    cout << " Antiderivatives: " << endl;
    for (char var = Monomial::VAR_MIN; var <= Monomial::VAR_MAX; var++) {
        cout << "  '" << var << "': " << polynomial->integrate(var) << endl;
    }

    return EXIT_SUCCESS;
}
