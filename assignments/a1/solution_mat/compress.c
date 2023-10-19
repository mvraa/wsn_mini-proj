// libraries
//#include "contiki.h"
#include <stdio.h> // printing
//#include <math.h> // math functions like sqrt, cosine, etc.
#include "sys/rtimer.h"

// signal length (N_), transform length (L) and number of DCT coefficients (M)
#define N_ 512
#define L 8
#define M 4

#define PI 3.14159265358979323846 // pi (used by cosine functions)

// second attempt
void dct_transform(double x[], double H[][L], double y[][N_/L]);
void select_coefficients(double y[][N_/L], double y_transmit[][N_/L]);
void reconstruct_signal(double y_transmit[][N_/L], double H[][L], double x_rec[]);
void print_array(double *a, int n); // print array
double compare_array(double *a, double *b, int n); // compare arrays (before and after transform)
int entry();
// math.h is taking too musch space so I implement my own math functions: 
double deg_to_rad(double deg);
double custom_abs(double x);
double custom_fact(int n);
double custom_cos(double deg);
double custom_fabs(double x);
double custom_sqrt(double x);
double custom_fmod(double x, double y);

/*---------------------------ENTRY PROCESS--------------------------------*/
PROCESS(dct_process, "dct process");
AUTOSTART_PROCESSES(&dct_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(dct_process, ev, data){
  static struct etimer timer;

  PROCESS_BEGIN();

  entry();

  /* Setup a periodic timer that expires after 10 seconds. */
  etimer_set(&timer, CLOCK_SECOND * 5);

  while(1) {
    //printf("Hello world!\n");

    /* Wait for the periodic timer to expire and then restart the timer. */
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    etimer_reset(&timer);
  }

  PROCESS_END();
}

int entry(){
    // input signal
    double x[N_] = {
      0.3500,0.3500,0.3500,0.3500,0.3500,0.3500,0.3500,0.3500,0.3500,0.0200,-0.2100,-0.3300,-0.3700,-0.3350,-0.2850,-0.2300,-0.2150,-0.2000,-0.1900,-0.2000,-0.2000,-0.2100,-0.2300,-0.2500,-0.2550,-0.2450,-0.2400,-0.2150,-0.2350,-0.2300,-0.2300,-0.2050,-0.1800,-0.1650,-0.1950,-0.2100,-0.2150,-0.1950,-0.1700,-0.1550,-0.1750,-0.1950,-0.2100,-0.2000,-0.2000,-0.1950,-0.1850,-0.1850,-0.1800,-0.1750,-0.1750,-0.1650,-0.1400,-0.1200,-0.1150,-0.1300,-0.1550,-0.1900,-0.1900,-0.1850,-0.1800,-0.1600,-0.1400,-0.1300,-0.1100,-0.1200,-0.1300,-0.1300,-0.1250,-0.0950,-0.0950,-0.0900,-0.0800,-0.0750,-0.0700,-0.0750,-0.0650,-0.0550,-0.0600,-0.0550,-0.0400,-0.0300,-0.0200,-0.0150,-0.0150,-0.0100,0.0150,0.0350,0.0250,0.0150,0.0150,0.0250,0.0250,0.0400,0.0400,0.0300,0.0150,0.0200,0.0350,0.0450,0.0500,0.0250,0.0200,0.0200,0.0100,0.0100,0,-0.0050,-0.0250,-0.0200,-0.0300,-0.0300,-0.0350,-0.0450,-0.0550,-0.0600,-0.0650,-0.0750,-0.0800,-0.0750,-0.0750,-0.0750,-0.0700,-0.0750,-0.0800,-0.0900,-0.1000,-0.0950,-0.0950,-0.0850,-0.0950,-0.1000,-0.1000,-0.1150,-0.1150,-0.1300,-0.1400,-0.1500,-0.1600,-0.1450,-0.1550,-0.1600,-0.1700,-0.1850,-0.1750,-0.1700,-0.1650,-0.1700,-0.1700,-0.1650,-0.1800,-0.1800,-0.2000,-0.2000,-0.2150,-0.2050,-0.2100,-0.2050,-0.2100,-0.2200,-0.2250,-0.2250,-0.2400,-0.2250,-0.2100,-0.2050,-0.1950,-0.1900,-0.2000,-0.1900,-0.1900,-0.1850,-0.1700,-0.1700,-0.1750,-0.1650,-0.1650,-0.1850,-0.1850,-0.2050,-0.2050,-0.2100,-0.1950,-0.1950,-0.1900,-0.1950,-0.1950,-0.1950,-0.1850,-0.1750,-0.1750,-0.2000,-0.2250,-0.2500,-0.2400,-0.2400,-0.2400,-0.2500,-0.2600,-0.2700,-0.2800,-0.3050,-0.3350,-0.3500,-0.3400,-0.2850,-0.2050,-0.1350,-0.0950,-0.0450,0.0100,0.0750,0.1400,0.1750,0.1350,0.0450,-0.1550,-0.4250,-0.7100,-0.9150,-1.0900,-1.2550,-1.3950,-1.4650,-1.5050,-1.4900,-1.4500,-1.3900,-1.3400,-1.2700,-1.1550,-1.0250,-0.9500,-0.9300,-0.9300,-0.9100,-0.8850,-0.8650,-0.8400,-0.8000,-0.7550,-0.7150,-0.6600,-0.6300,-0.5800,-0.5400,-0.5100,-0.4650,-0.4350,-0.3950,-0.3650,-0.3150,-0.2650,-0.2000,-0.1550,-0.0900,-0.0400,-0.0150,-0.0100,0.0050,0.0300,0.0550,0.0800,0.0850,0.0900,0.1100,0.1100,0.1300,0.1500,0.1550,0.1500,0.1800,0.1850,0.2050,0.2200,0.2350,0.2350,0.2550,0.2700,0.2900,0.2900,0.3000,0.3000,0.3050,0.3250,0.3150,0.3100,0.3150,0.3150,0.3450,0.3600,0.3700,0.3750,0.3750,0.3650,0.3750,0.3850,0.4100,0.4050,0.4000,0.4000,0.4050,0.4000,0.4100,0.3950,0.4000,0.3850,0.3900,0.3950,0.3850,0.3800,0.3550,0.3550,0.3550,0.3500,0.3250,0.3000,0.2800,0.2500,0.2500,0.2500,0.2500,0.2350,0.2100,0.1750,0.1650,0.1600,0.1450,0.1400,0.1450,0.1300,0.1000,0.0800,0.0400,0.0150,-0.0100,-0.0250,-0.0250,-0.0300,-0.0400,-0.0450,-0.0700,-0.1050,-0.0800,-0.0750,-0.0850,-0.0900,-0.1100,-0.1300,-0.1200,-0.1150,-0.1250,-0.1500,-0.1700,-0.1600,-0.1600,-0.1500,-0.1550,-0.1600,-0.1700,-0.1850,-0.1800,-0.1800,-0.1900,-0.1800,-0.1700,-0.1600,-0.1400,-0.1400,-0.1450,-0.1700,-0.1800,-0.1750,-0.1600,-0.1550,-0.1550,-0.1650,-0.1500,-0.1550,-0.1500,-0.1450,-0.1450,-0.1650,-0.1950,-0.2050,-0.1900,-0.1800,-0.1650,-0.1700,-0.1600,-0.1700,-0.1650,-0.1700,-0.1700,-0.1700,-0.1700,-0.1850,-0.1900,-0.1800,-0.1800,-0.1800,-0.1750,-0.2000,-0.2100,-0.2000,-0.2000,-0.2100,-0.2150,-0.2100,-0.2000,-0.1750,-0.1600,-0.1450,-0.1450,-0.1350,-0.1400,-0.1350,-0.1300,-0.1200,-0.1150,-0.1100,-0.1050,-0.1000,-0.0950,-0.1100,-0.1400,-0.1750,-0.1800,-0.1800,-0.1800,-0.1650,-0.1750,-0.1750,-0.1750,-0.1650,-0.1650,-0.1650,-0.1750,-0.1650,-0.1600,-0.1550,-0.1500,-0.1550,-0.1600,-0.1600,-0.1550,-0.1450,-0.1500,-0.1500,-0.1850,-0.2000,-0.2000,-0.1950,-0.1900,-0.1900,-0.1950,-0.2000,-0.1950,-0.1850,-0.1750,-0.1750,-0.1550,-0.1700,-0.1700,-0.1650,-0.1500,-0.1600,-0.1650,-0.1750,-0.1700,-0.1700,-0.1850,-0.2150,-0.2250,-0.1900,-0.1250,-0.0400,0.0500,0.1250,0.2150,0.3200,0.4150,0.5000,0.6050,0.7250,0.8450,0.9400,1.0600,1.1100,1.1000,1.0050,0.8600,0.6150,0.2400,-0.1100,-0.3350,-0.4600,-0.5150,-0.5400,-0.5100,-0.4400,-0.3850,-0.3600,-0.3500,-0.3350,-0.3200,-0.2950,-0.3000,-0.3000,-0.3250,-0.3350,-0.3350
    };
    double H[L][L]; // DCT matrix
    double y[L][N_/L]; // DCT transformed signal
    double y_transmit[M][N_/L]; // selected coefficients
    double x_rec[N_]; // reconstructed signal

    rtimer_clock_t t1, t2; // execution timestamps

    t1 = RTIMER_NOW();
    dct_transform(x, H, y); // DCT transform of signal
    t2 = RTIMER_NOW();
    rtimer_clock_t dct_time = t2 - t1;
    
    t1 = RTIMER_NOW();
    select_coefficients(y, y_transmit); // Select first M coefficients for each DCT transformed signal
    t2 = RTIMER_NOW();
    rtimer_clock_t coef_time = t2 - t1;

    t1 = RTIMER_NOW();
    reconstruct_signal(y_transmit, H, x_rec); // Reconstruct the original signal
    t2 = RTIMER_NOW();
    rtimer_clock_t recon_time = t2 - t1;

    // print and compare:
    printf("Original signal:\n");
    print_array(x, N_);
    printf("Recovered signal:\n");
    print_array(x_rec, N_);
    printf("Execution time: %lu us\n", (unsigned long)(dct_time + coef_time + recon_time));
    double mse = compare_array(x, x_rec, N_);
    printf("Signal quality (Mean squared error): %f\n", mse);
    double cr = L / M; // compression ratio
    printf("Compression ratio: %f\n", cr);

    return 0;
}

// Function to perform DCT transform of signal using matrix multiplication
void dct_transform(double x[], double H[][L], double y[][N_/L]){
    // Compute the DCT matrix using the formula H[i][j] = sqrt(2/L) * cos(pi*(2*i+1)*j/(2*L))
    for (int i = 0; i < L; i++){
        for (int j = 0; j < L; j++){
            H[i][j] = custom_sqrt(2.0 / L) * custom_cos(PI * (2.0 * i + 1.0) * j / (2.0 * L));
        }
    }

    // Multiply each L-length segment of x with H and store in y using the formula y[i][j] = sum(H[i][k] * x[L*j+k]) for k = 0 to L-1
    for (int i = 0; i < L; i++){
        for (int j = 0; j < N_ / L; j++){
            y[i][j] = 0.0;
            for (int k = 0; k < L; k++){
                y[i][j] += H[i][k] * x[L * j + k];
            }
        }
    }
}

// Function to select first M coefficients for each DCT transformed signal and store in y_transmit array
void select_coefficients(double y[][N_/L], double y_transmit[][N_/L]){
    // Copy first M rows of y to y_transmit array
    for (int i = 0; i < M; i++){
        for (int j = 0; j < N_ / L; j++){
            y_transmit[i][j] = y[i][j];
        }
    }
}

// Function to reconstruct the original signal from the selected coefficients using inverse DCT transform
void reconstruct_signal(double y_transmit[][N_/L], double H[][L], double x_rec[]){
    double invH[L][L]; // inverse DCT matrix
    double y_receive[L][N_/L]; // received coefficients

    // Compute the inverse DCT matrix using the formula invH[i][j] = H[j][i]
    for (int i = 0; i < L; i++){
        for (int j = 0; j < L; j++){
            invH[i][j] = H[j][i];
        }
    }

    // Set the first M rows of y_receive to y_transmit and the rest to zero
    for (int i = 0; i < L; i++){
        for (int j = 0; j < N_ / L; j++){
            if (i < M) y_receive[i][j] = y_transmit[i][j];
            else y_receive[i][j] = 0.0;
        }
    }

    // Multiply each L-length segment of y_receive with invH and store in x_rec using the formula x_rec[L*j+k] = sum(invH[k][i] * y_receive[i][j]) for i = 0 to L-1
    for (int j = 0; j < N_ / L; j++){
        for (int k = 0; k < L; k++){
            x_rec[L * j + k] = 0.0;
            for (int i = 0; i < L; i++){
                x_rec[L * j + k] += invH[k][i] * y_receive[i][j];
            }
        }
    }
}

// Function to compare two arrays of length n and return the mean squared error
double compare_array(double *a, double *b, int n){
    double err = 0;
    for (int i = 0; i < n; i++){
        err += (a[i] - b[i]) * (a[i] - b[i]);
    }
    return err/n;
}

void print_array(double *a, int n){
    for (int i = 0; i < n; i++){
        printf("%f ", a[i]);
    }
    printf("\n");
}

////////////////// math stuff ////////////////////

double deg_to_rad(double deg) {
    return deg * PI / 180.0;
}

// Calculate the absolute value of a number
double custom_abs(double x) {
    if (x < 0) return -x;
    else return x;
}

// Calculate the factorial of a non-negative integer
double custom_fact(int n) {
    // Initialize the result variable
    double result = 1.0;

    // Loop from 1 to n and multiply the result by each number
    for (int i = 1; i <= n; i++) {
        result *= i;
    }

    // Return the result as the factorial value
    return result;
}

// Define the custom fabs function
double custom_fabs(double x) {
    // Check if x is negative and return -x
    if (x < 0) return -x;
    else return x;
}

// A custom fmod function that uses the Euclidean algorithm
// fmod(x, y) = x - y * floor(x / y)
double custom_fmod(double x, double y) {
  // Check if y is zero
  if (y == 0) {
    //return NAN; // Not a number
  }
  // Check if x and y have the same sign
  if ((x > 0 && y > 0) || (x < 0 && y < 0)) {
    // Use the Euclidean algorithm to find the remainder
    while (custom_fabs(x) >= custom_fabs(y)) {
      x = x - y;
    }
    // Return the final remainder
    return x;
  } else {
    // Use the Euclidean algorithm to find the remainder with the opposite sign
    while (custom_fabs(x) >= custom_fabs(y)) {
      x = x + y;
    }
    // Return the final remainder with the opposite sign
    return -x;
  }
}

// A custom cos function that uses the Taylor series expansion
// cos(x) = 1 - x^2/2! + x^4/4! - x^6/6! + ...
double custom_cos(double x) {
  // Normalize x to be in the range [-pi, pi]
  x = custom_fmod(x, 2 * PI);
  if (x < -PI) {
    x += 2 * PI;
  } else if (x > PI) {
    x -= 2 * PI;
  }
  // Initialize the sum, term, and sign
  double sum = 0;
  double term = 1;
  int sign = 1;
  // Loop until the term is smaller than a small epsilon
  for (int n = 0; term > 1e-15; n++) {
    // Add the term to the sum with the appropriate sign
    sum += sign * term;
    // Update the sign to alternate between positive and negative
    sign = -sign;
    // Update the term to the next power of x divided by the next factorial
    term = term * x * x / ((2 * n + 1) * (2 * n + 2));
  }
  // Return the final sum
  return sum;
}

// Define the custom sqrt function
double custom_sqrt(double x) {
    // Check if x is non-positive and return 0
    if (x <= 0) {
        return 0;
    }

    // Initialize the guess variable with x
    double guess = x;

    // Initialize the epsilon value for the error tolerance
    double epsilon = 1e-6;

    // Loop until the absolute difference between guess^2 and x is smaller than epsilon
    while (custom_fabs(guess * guess - x) > epsilon) {
        // Update the guess by averaging it with x / guess
        guess = (guess + x / guess) / 2.0;
    }

    // Return the guess as the square root value
    return guess;
}