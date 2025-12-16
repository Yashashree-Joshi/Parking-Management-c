#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 10
#define RATE_2W 10
#define RATE_4W 20

int b_arr[MAX] = {0};
int b_arrf[MAX] = {0};
int ctr1 = 0, ctr2 = 0;

time_t entry_time_2w[MAX] = {0};
time_t entry_time_4w[MAX] = {0};

/* ---------- SAVE DATA TO TXT ---------- */
void save_data() {
    FILE *fp = fopen("parking_state.txt", "w");
    if (!fp) return;

    fprintf(fp, "%d %d\n", ctr1, ctr2);

    for (int i = 0; i < MAX; i++)
        fprintf(fp, "%d %ld\n", b_arr[i], entry_time_2w[i]);

    for (int i = 0; i < MAX; i++)
        fprintf(fp, "%d %ld\n", b_arrf[i], entry_time_4w[i]);

    fclose(fp);
}

/* ---------- LOAD DATA FROM TXT ---------- */
void load_data() {
    FILE *fp = fopen("parking_state.txt", "r");
    if (!fp) return;

    fscanf(fp, "%d %d", &ctr1, &ctr2);

    for (int i = 0; i < MAX; i++)
        fscanf(fp, "%d %ld", &b_arr[i], &entry_time_2w[i]);

    for (int i = 0; i < MAX; i++)
        fscanf(fp, "%d %ld", &b_arrf[i], &entry_time_4w[i]);

    fclose(fp);
}

/* ---------- BILL LOG ---------- */
void log_billing(int type, int num, int hrs, int amt) {
    FILE *fp = fopen("billing_history.txt", "a");
    if (!fp) return;

    fprintf(fp, "%s | %d | %d hrs | Rs %d | %s",
            (type == 2) ? "2W" : "4W",
            num, hrs, amt, ctime(&(time_t){time(NULL)}));

    fclose(fp);
}

/* ---------- LOGIN ---------- */
int login() {
    char user[20], pass[20];
    int attempts = 0;
    FILE *log = fopen("login_log.txt", "a");

    while (attempts < 3) {
        printf("Username: ");
        scanf("%s", user);
        printf("Password: ");
        scanf("%s", pass);

        if (strcmp(user, "Admin") == 0 && strcmp(pass, "pass1") == 0) {
            printf("Login successful\n");
            if (log) fprintf(log, "SUCCESS\n");
            load_data();
            fclose(log);
            return 1;
        } else {
            printf("Login failed\n");
            if (log) fprintf(log, "FAILED\n");
            attempts++;
        }
    }

    if (log) fclose(log);
    return 0;
}

/* ---------- INSERT VEHICLE ---------- */
void insert_car() {
    int d, num;
    printf("Vehicle Type (2/4): ");
    scanf("%d", &d);
    printf("Vehicle Number: ");
    scanf("%d", &num);

    if (d == 2) {
        for (int i = 0; i < MAX; i++) {
            if (b_arr[i] == 0) {
                b_arr[i] = num;
                entry_time_2w[i] = time(NULL);
                ctr1++;
                save_data();
                printf("2W parked at slot %d\n", i + 1);
                return;
            }
        }
        printf("2W Parking Full\n");
    }
    else if (d == 4) {
        for (int i = 0; i < MAX; i++) {
            if (b_arrf[i] == 0) {
                b_arrf[i] = num;
                entry_time_4w[i] = time(NULL);
                ctr2++;
                save_data();
                printf("4W parked at slot %d\n", i + 1);
                return;
            }
        }
        printf("4W Parking Full\n");
    }
}

/* ---------- DISPLAY ---------- */
void disp_2() {
    printf("\n2W Parking:\n");
    for (int i = 0; i < MAX; i++)
        printf("%d ", b_arr[i]);
    printf("\nTotal: %d\n", ctr1);
}

void disp_4() {
    printf("\n4W Parking:\n");
    for (int i = 0; i < MAX; i++)
        printf("%d ", b_arrf[i]);
    printf("\nTotal: %d\n", ctr2);
}

/* ---------- EXIT + BILL ---------- */
void exit_vehicle() {
    int d, num;
    time_t exit_time = time(NULL);

    printf("Vehicle Type (2/4): ");
    scanf("%d", &d);
    printf("Vehicle Number: ");
    scanf("%d", &num);

    if (d == 2) {
        for (int i = 0; i < MAX; i++) {
            if (b_arr[i] == num) {
                double sec = difftime(exit_time, entry_time_2w[i]);
                int hrs = (sec <= 3600) ? 1 : (int)((sec + 3599) / 3600);
                int bill = hrs * RATE_2W;

                printf("\nEntry: %s", ctime(&entry_time_2w[i]));
                printf("Exit : %s", ctime(&exit_time));
                printf("Bill : Rs %d\n", bill);

                log_billing(2, num, hrs, bill);

                b_arr[i] = 0;
                entry_time_2w[i] = 0;
                ctr1--;
                save_data();
                return;
            }
        }
    }

    else if (d == 4) {
        for (int i = 0; i < MAX; i++) {
            if (b_arrf[i] == num) {
                double sec = difftime(exit_time, entry_time_4w[i]);
                int hrs = (sec <= 3600) ? 1 : (int)((sec + 3599) / 3600);
                int bill = hrs * RATE_4W;

                printf("\nEntry: %s", ctime(&entry_time_4w[i]));
                printf("Exit : %s", ctime(&exit_time));
                printf("Bill : Rs %d\n", bill);

                log_billing(4, num, hrs, bill);

                b_arrf[i] = 0;
                entry_time_4w[i] = 0;
                ctr2--;
                save_data();
                return;
            }
        }
    }

    printf("Vehicle not found\n");
}

/* ---------- SEARCH ---------- */
void search() {
    int d, num;
    printf("Vehicle Type (2/4): ");
    scanf("%d", &d);
    printf("Vehicle Number: ");
    scanf("%d", &num);

    if (d == 2)
        for (int i = 0; i < MAX; i++)
            if (b_arr[i] == num)
                printf("Found at slot %d\n", i + 1);

    if (d == 4)
        for (int i = 0; i < MAX; i++)
            if (b_arrf[i] == num)
                printf("Found at slot %d\n", i + 1);
}

/* ---------- MENU ---------- */
void menu() {
    int ch;
    while (1) {
        printf("\n1.Arrival\n2.Display 2W\n3.Display 4W\n4.Exit\n5.Search\n6.Exit\nChoice: ");
        scanf("%d", &ch);

        switch (ch) {
            case 1: insert_car(); break;
            case 2: disp_2(); break;
            case 3: disp_4(); break;
            case 4: exit_vehicle(); break;
            case 5: search(); break;
            case 6: save_data(); exit(0);
            default: printf("Invalid choice\n");
        }
    }
}

int main() {
    if (login())
        menu();
    return 0;
}
