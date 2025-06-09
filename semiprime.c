#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Function to check if a number is semiprime
// รับค่า n และตัวแปรสำหรับเก็บ factor1 และ factor2
// *factor1 = i;
// *factor2 = n / i;
// rank และ size ใช้สำหรับการแบ่งงานใน MPI
int is_semiprime(long long n, long long *factor1, long long *factor2, int rank, int size) {
    long long start, end;
    long long root = (long long) sqrt((double) n);
    long long chunk = root / size;
    // กำหนดช่วงที่แต่ละ process จะทำงาน
    start = rank * chunk + 2;
    end = (rank == size - 1) ? root : (start + chunk - 1);

    // loop เพื่อหาค่า factor ของ n
    for (long long i = start; i <= end; i++) {
        if (n % i == 0) {
            *factor1 = i;
            *factor2 = n / i;
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv); //ส่ง argc และ argv ไปให้ MPI เพื่อให้ MPI สามารถจัดการกับ argument ได้
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long long n = atoll(argv[1]); // รับค่า n จาก argument ของโปรแกรม
    double start_time, end_time;
    if (rank == 0) start_time = MPI_Wtime();

    long long local_factor1 = 0, local_factor2 = 0; // ตัวแปรสำหรับเก็บ factor ที่พบในแต่ละ process

    // เรียกใช้ฟังก์ชัน is_semiprime เพื่อหาค่า factor ของ n
    // local_found จะเป็น 1 ถ้า n เป็น semiprime และ 0 ถ้าไม่ใช่
    int local_found = is_semiprime(n, &local_factor1, &local_factor2, rank, size);

    int global_found;
    // ใช้ MPI_Reduce เพื่อรวมผลลัพธ์จากทุก process
    // MPI_LOR ใช้สำหรับการรวมผลลัพธ์แบบ logical OR
    // ถ้า process ใดพบว่า n เป็น semiprime จะทำให้ global_found เป็น 1
    MPI_Reduce(&local_found, &global_found, 1, MPI_INT, MPI_LOR, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        long long factor1 = 0, factor2 = 0;
        if (global_found) {
            MPI_Status status;
            int flag_found_process = -1;
            // ตรวจสอบว่า process ใดพบว่า n เป็น semiprime
            // ถ้า process 0 พบว่า n เป็น semiprime จะใช้ค่า local_factor1 และ local_factor2
            // ถ้าไม่พบ จะใช้ค่า factor1 และ factor2 ที่ได้รับจาก process อื่น
            for (int i = 0; i < size; i++) {
                int found_i = 0;
                if (i == 0) found_i = local_found;
                else {
                    MPI_Recv(&found_i, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
                }
                if (found_i) {
                    flag_found_process = i;
                    break;
                }
            }
            // ถ้า process 0 ไม่พบว่า n เป็น semiprime จะใช้ค่า local_factor1 และ local_factor2
            if (flag_found_process == 0) {
                factor1 = local_factor1;
                factor2 = local_factor2;
            } else {
                // ถ้า process อื่นพบว่า n เป็น semiprime จะรับค่า factor1 และ factor2 จาก process นั้น
                MPI_Recv(&factor1, 1, MPI_LONG_LONG, flag_found_process, 1, MPI_COMM_WORLD, &status);
                MPI_Recv(&factor2, 1, MPI_LONG_LONG, flag_found_process, 2, MPI_COMM_WORLD, &status);
            }
            printf("%lld is a semiprime: %lld = %lld * %lld\n", n, n, factor1, factor2);
        } else {
            printf("%lld is NOT a semiprime\n", n);
        }
        end_time = MPI_Wtime();
        printf("Execution time: %.6f seconds\n", end_time - start_time);
    } else {
        MPI_Send(&local_found, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        if (local_found) {
            MPI_Send(&local_factor1, 1, MPI_LONG_LONG, 0, 1, MPI_COMM_WORLD);
            MPI_Send(&local_factor2, 1, MPI_LONG_LONG, 0, 2, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}
