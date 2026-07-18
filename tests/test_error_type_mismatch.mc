// Test program: Semantic Error - Type mismatch
int x;
bool flag;

x = 5;
flag = x + 3;  // Error: cannot assign int to bool
