Semiprime
Semiprime คือจำนวนเต็มที่เกิดจากผลคูณของจำนวนเฉพาะสองตัว (อาจซ้ำกันได้) 
15 = 3 × 5 

49 = 7 × 7 

12 = 3 × 4 ❌ (4 ไม่ใช่จำนวนเฉพาะ)

หาค่า Semiprime ของตัวเลขที่กำหนด
เขียนภาษา C และใช้ MPI (Message Passing Interface) 
เพื่อรันโปรแกรมแบบขนาน (parallel computing) สำหรับตรวจสอบว่าเลขจำนวนเต็ม n ที่รับเข้ามานั้นเป็น semiprime ไหม

compile
mpicc -o semiprime semiprime.c -lm -std=c99

run
mpirun -np 4 ./semiprime 10000007

🦉🌲

