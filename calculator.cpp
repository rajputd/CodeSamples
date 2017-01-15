#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include <math.h>
using namespace std;

enum Function {Add, Sub, Mul, Div, Exp, Log, Sin, Cos, Num};

class Expression {
  private:
    Expression* right;
    Expression* left;
    Function type;
    float value;
  public:
    Expression(string func) {
        if(func == "+") {
          type = Add;
        } else if (func == "-") {
          type = Sub;
        } else if (func == "*") {
          type = Mul;
        } else if (func == "/") {
          type = Div;
        } else if (func == "^") {
          type = Exp;
        } else if (func == "log") {
          type = Log;
          left = NULL;
        } else if (func == "sin") {
          type = Sin;
          left = NULL;
        } else if (func == "cos") {
          type = Cos;
          left = NULL;
        } else {
          type = Num;
          value = stof(func);
          left = NULL;
          right = NULL;
        }
      }

    void setRight(Expression* address) {
      right = address;
    }

    void setLeft(Expression* address) {
      left = address;
    }

    void setType(Function value) {
      type = value;
    }

    Expression* getRight() {
      return right;
    }

    Expression* getLeft() {
      return left;
    }

    float evaluate() {
      switch (type) {
        case Add: return left->evaluate() + right->evaluate();
        case Sub: return left->evaluate() - right->evaluate();
        case Mul: return left->evaluate() * right->evaluate();

        case Div: if(right->evaluate() != 0) {
                    return left->evaluate() / right->evaluate();
                  } else {
                    //cannot divide by zero
                    throw 1;
                  };

        case Exp: return pow(left->evaluate(), right->evaluate());
        case Log: return log(right->evaluate());
        case Sin: return sin(right->evaluate());
        case Cos: return cos(right->evaluate());
        case Num: return value;
      }
    }
};

vector<string> shuntingyard(string input);
vector<string> tokenize(string input);
bool checkFunction(string token);
Expression* makeTree(vector<string> &input);
void deleteExpression(Expression* root);

int main() {

    string input;
    bool settingVariable;
    bool undeclaredVariable;
    unordered_map<string, float> variables;
    string varName;

    //set constants
    variables.insert(make_pair<string, float>("e", 2.718));
    variables.insert(make_pair<string, float>("PI", 3.14169));

    while(true) {
      //get input
      getline(cin, input);

      //end program if quit
      if(input == "quit") {
        break;
      }

      //if setting a variable
      if(input.substr(0, 4) == "let ") {
        settingVariable = true;

        //delete "let " from string
        input.replace(0,4,"");

        //get variable name
        varName = "";
        while(input[0] != '=') {
          if(input[0] != ' ') {
            varName += input[0];
          }
          input.erase(0,1);
        }

        //delete "=" from string
        input.erase(0,1);
      }

      //convert expression into postfix
      vector<string> out = shuntingyard(input);

      //prepare to check for undeclared variables
      undeclaredVariable = false;
      int size = out.size();

      for(int i = 0; i < size; i++) {
        if(!checkFunction(out[i]) && !isdigit(out[i][0])) { //if a variable
              if(variables.find(out[i]) == variables.end()) {
                // if variable not found
                undeclaredVariable = true;
              } else {
                // replace variable with its associated value
                out[i] = to_string(variables[out[i]]);
              }
            }
        }

        //print Undeclared-Variable output
        if(undeclaredVariable) {
          cout << "Undeclared-Variable\n";
          continue;
        }


      //create expression tree
      Expression* root = makeTree(out);

      //store value into variable if setting a variable
      if(settingVariable) {
          variables[varName] = root->evaluate();
          settingVariable = false;
      } else {
          //else print evaluated value of expression
          try {
              cout << root->evaluate() << "\n";
          } catch (int e) {
            cout << "Division-By-Zero\n";
          }
      }
    }

    return 0;
}

//converts exression from infix to postfix
vector<string> shuntingyard(string input) {

	vector<string> output;
	vector<string> operators;
	vector<string> tokens = tokenize(input);

	int size = tokens.size();

  for(int i = 0; i < size; i++) {
	  if(checkFunction(tokens[i])) {
			if(!operators.empty()) {
				if((tokens[i] == "+" || tokens[i] == "-") && operators.back() != "(") {
					//pop top of stack to output
					output.push_back(operators.back());
					operators.pop_back();
				} else if ((tokens[i] == "*" || tokens[i] == "/") && operators.back() != "+" && operators.back() != "-"  && operators.back() != "(") {
					//pop top of stack to output
					output.push_back(operators.back());
					operators.pop_back();
				}
			}

      //push function to stack
			operators.push_back(tokens[i]);

	 } else if (tokens[i] == "(") {
     operators.push_back("(");
   } else if (tokens[i] == ")") {
     //while ( not found pop functions to output
     while(operators.back() != "(" && !operators.empty()) {
       output.push_back(operators.back());
       operators.pop_back();
     }

      //delete remaining "("
      operators.pop_back();
   } else {
		output.push_back(tokens[i]);
	 }
  }

  //empty all operators left in stack to output
  while(!operators.empty()) {
    output.push_back(operators.back());
    operators.pop_back();
  }

    return output;
}

//tokenizes a expression
//tokenizes unrary expressions into smaller pieces
//ex. sin(43) -> [sin, (, 43, )]
vector<string> tokenize(string input) {

  vector<string> tokens;
  string token = "";
	int size = input.size();

	for(int i = 0; i < size; i++) {
		if(input[i] == ' ' || input[i] == '\t') {
			//ignore spaces
			continue;
		} else if (input[i] == '(') {
			tokens.push_back("(");
			continue;
		} else if (input[i] == ')') {
			//store number that was infront of this, if any
			if (!token.empty()) {
				tokens.push_back(token);
				token.clear();
			}
			tokens.push_back(")");
			continue;
		} else if (input[i] == '+' || input[i] == '-' || input[i] == '*' || input[i] == '/' || input[i] == '^') {
			//store number that was infront of this, if any
			if (!token.empty()) {
				tokens.push_back(token);
				token.clear();
			}

			//store binary operator
			token += input[i];
			tokens.push_back(token);
			token.clear();
			continue;
		}

		//append to token if it is a number or variable
		token += input[i];

		//store unary operator
		if((token == "log" || token == "sin" || token == "cos")) {
      int j = i + 1;
      while(j < size) {
        if(input[j] == '(') {
          tokens.push_back(token);
    			token.clear();
          break;
        } else if (input[j] == ' ') {
          j++;
          continue;
        } else {
          break;
        }
      }
		}
	}

	//store last number in input if there is one
	if(!token.empty()){
		tokens.push_back(token);
	}

    return tokens;
}

//check if string is a valid function
bool checkFunction(string token) {

return token == "+" ||
	   token == "-" ||
	   token == "*" ||
	   token == "/" ||
	   token == "^" ||
	   token == "log" ||
	   token == "sin" ||
	   token == "cos";
}

/*
*recursively creates an expression tree from a vector containing a postfix
expression
*/
Expression* makeTree(vector<string> &input) {
  Expression* output = new Expression(input.back());

  //if a binary operator create a node with two children
  if(input.back() == "+" || input.back() == "-" || input.back() == "*" || input.back() == "/" || input.back() == "^") {
    input.pop_back();
    output->setRight(makeTree(input));
    output->setLeft(makeTree(input));

  //if a unary operator create a node with one child
  } else if (input.back() == "log" || input.back() == "cos" ||input.back() == "sin") {
    input.pop_back();
    output->setRight(makeTree(input));

  //if a number create node with no children
  } else {
    input.pop_back();
  }

  return output;
}
