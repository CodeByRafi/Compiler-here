// Example: Nested scopes
int x;
x = 10;

{
    int y;
    y = 20;
    print y;  // y is visible here
}

// y is not visible here - will error if we try to use it
print x;
