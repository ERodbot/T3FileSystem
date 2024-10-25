#ifndef UTILS
#define UTILS
// Function to check if a number is prime
int is_prime(int num) {
    if (num <= 1) {
        return 0; // Not prime
    }
    if (num == 2) {
        return 1; // 2 is the only even prime number
    }
    if (num % 2 == 0) {
        return 0; // Other even numbers are not prime
    }

    // Check divisibility from 3 up to the square root of the number
    for (int i = 3; i <= sqrt(num); i += 2) {
        if (num % i == 0) {
            return 0; // Not prime if divisible
        }
    }

    return 1; // Number is prime
}

// Function to find the nearest prime number greater than x
int next_prime(int x) {
    int candidate = x + 1; // Start checking from the next number
    while (!is_prime(candidate)) {
        candidate++;
    }
    return candidate;
}
#endif // !UTILS
