/*Commands to run each program*/
g++ -o prog1 ass4_1.cpp
g++ -o prog2 ass4_2.cpp -lpthread
gcc -o prog3 ass4_3.c -pthread

Assumptions for program 2:
The questions are multiple choice question and only one option is correct.No negative marks for incorrect answer and each question carries 1 mark.

Instructions to run program 2:
For Question2 pass the inputs (number of question, number of student) and one optional argument num_options (default 4) as command line arguments and also the .txt file consisting of the questions and correct answer key

The .txt file contains num_q + 1 number of lines.first num_q lines consist of all questions with options and the last line consist of correct keys for each question. It consist of characters for num_options=4 option each character can take value from A - D.The last line should not contain any space separated inputs it consist of num_q number of characters all in capital letters without space.

Example 1:
g++ -o prog2 ass4_2.cpp -lpthread
./prog2 7 5 < in.txt

Contents in in.txt
ques1? (A)opt1 (B)opt2 (C)opt3 (D)opt4 
ques2? (A)opt1 (B)opt2 (C)opt3 (D)opt4 
ques3? (A)opt1 (B)opt2 (C)opt3 (D)opt4 
ques4? (A)opt1 (B)opt2 (C)opt3 (D)opt4 
ques5? (A)opt1 (B)opt2 (C)opt3 (D)opt4 
ques6? (A)opt1 (B)opt2 (C)opt3 (D)opt4 
ques7? (A)opt1 (B)opt2 (C)opt3 (D)opt4 
CBBACDA

First argument is number of questions, second argument is number of students taking exam or number of child processes

Example 2:
./prog2 7 5 3 < in.txt
Contents in in.txt
ques1? (A)opt1 (B)opt2 (C)opt3 
ques2? (A)opt1 (B)opt2 (C)opt3 
ques3? (A)opt1 (B)opt2 (C)opt3 
ques4? (A)opt1 (B)opt2 (C)opt3 
ques5? (A)opt1 (B)opt2 (C)opt3 
ques6? (A)opt1 (B)opt2 (C)opt3 
ques7? (A)opt1 (B)opt2 (C)opt3 
CBBACBA

Third optional argument is number of options
Note: Inconsistent input may result in erroneous outputs

