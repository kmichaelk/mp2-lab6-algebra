﻿#include <iostream>

#include "polynomial.h"

using namespace std;

void perform_arithmetic_operation(Polynomial& polynomial, char opcode, string& buf);

int main()
{
    cout << "--------------------------" << endl;
    cout << "     Polynomial Shell     " << endl;
    cout << "--------------------------" << endl;
    cout << endl;

    cout << " ? Arithmetics: +, -, *, / [polynomial]" << endl;
    cout << " ? Operations: differentiate [variable], integrate [variable], calculate [x] [y] [z]" << endl;
    cout << " ? Enter 'discard' to input another polynomial" << endl;
    cout << endl;

    Monomial::Point point = {
            { 'x', 0 },
            { 'y', 0 },
            { 'z', 0 },
    };

    std::shared_ptr<Polynomial> polynomial;

    string input;
    char var;
    while (true) {
        cout << ">>> ";
        getline(cin, input);

        if ("exit" == input) {
            break;
        }

        try {
            polynomial = std::make_shared<Polynomial>(input);
        } catch (const expression_parse_error& err) {
            cout << "Malformed input: " << err.what() << endl;
            continue;
        }
        polynomial->compact();

        cout << "<<< " << polynomial << endl;

        while (true) {
            cout << " Enter action: ";
            cin >> input;

            if ("+" == input || "-" == input || "*" == input || "/" == input) {
                perform_arithmetic_operation(*polynomial, input[0], input);
            } else if ("differentiate" == input || "integrate" == input) {
                cin >> var;
                if (var < Monomial::VAR_MIN || var > Monomial::VAR_MAX) {
                    cout << "Unknown variable";
                } else {
                    if ("differentiate" == input) {
                        *polynomial = polynomial->differentiate(var);
                    } else {
                        *polynomial = polynomial->integrate(var);
                    }
                    cout << "<<< " << polynomial << endl;
                }
            } else if ("evaluate" == input) {
                for (auto& item : point) {
                    cin >> item.second;
                }
                cout << "<<< " << polynomial->calculate(point) << endl;
            } else if ("discard" == input) {
                cin.ignore();
                break;
            } else {
                cout << "Invalid action" << endl;
            }
        }
    }

    return EXIT_SUCCESS;
}

void perform_arithmetic_operation(Polynomial& polynomial, char opcode, string& buf)
{
    getline(cin, buf);

    std::shared_ptr<Polynomial> other;
    try {
        other = std::make_shared<Polynomial>(buf);
    } catch (const expression_parse_error& err) {
        cout << "Malformed input: " << err.what() << endl;
    }

    cout << "<<< ";

    switch (opcode) {
        case '+': {
            polynomial += *other;
            break;
        }
        case '-': {
            polynomial -= *other;
            break;
        }
        case '*': {
            polynomial *= *other;
            break;
        }
        case '/': {
            polynomial /= *other;
            break;
        }
        default: {
            throw runtime_error("assertion failure: unknown opcode");
        }
    }

    cout << polynomial << endl;
}
