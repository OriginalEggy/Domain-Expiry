#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_DATE_SIZE 50

int calculateDaysToExpiration(const char* expirationDate) {
    // Get the current date
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    int currentYear = tm.tm_year + 1900;
    int currentMonth = tm.tm_mon + 1;
    int currentDay = tm.tm_mday;

    // Parse the expiration date
    int expirationYear, expirationMonth, expirationDay;
    sscanf(expirationDate, "%d-%d-%d", &expirationYear, &expirationMonth, &expirationDay);

    // Calculate the number of days to expiration
    int daysToExpiration = (expirationYear - currentYear) * 365 +
                           (expirationMonth - currentMonth) * 30 +
                           (expirationDay - currentDay);
    
    return daysToExpiration;
}

int main(int argc, char* argv[]) {
    // Check if the correct number of command-line arguments is provided
    if (argc != 2) {
        printf("Usage: %s <domain>\n", argv[0]);
        return 1;
    }

    // Prepare the WHOIS command
    char command[100];
    snprintf(command, sizeof(command), "whois %s | findstr /C:\"Expiry Date\"", argv[1]);

    // Run the WHOIS command and read the output
    FILE* fp = _popen(command, "r");
    if (fp == NULL) {
        printf("Error running WHOIS command.\n");
        return 1;
    }

    char expirationDate[MAX_DATE_SIZE];
    if (fgets(expirationDate, sizeof(expirationDate), fp) == NULL) {
        printf("Error reading WHOIS output.\n");
        _pclose(fp);
        return 1;
    }

    _pclose(fp);

    // Extract the expiration date from the WHOIS output
    char* dateStart = strchr(expirationDate, ':');
    if (dateStart == NULL) {
        printf("Expiration date not found.\n");
        return 1;
    }
    dateStart++;  // Move past the colon
    char* dateEnd = strchr(dateStart, '\n');
    if (dateEnd == NULL) {
        printf("Invalid expiration date format.\n");
        return 1;
    }
    *dateEnd = '\0';  // Null-terminate the date string

    // Calculate the number of days to expiration
    int daysToExpiration = calculateDaysToExpiration(dateStart);

    // Print the result
    printf("%d\n", daysToExpiration);

    return 0;
}
