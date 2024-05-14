#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "terminal.h"
#include "keyboard.h"
#include "rtc.h"
#include "rofile.h"


#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

/* Divide by Zero Test - Example
 * 
 * Asserts that you cannot divide by 0
 * Inputs: None
 * Outputs: PASS/FAIL, undefined integer
 * Side Effects: throws exception message and freezes OS
 * Coverage: Exception Handling
 */
int dividebyzero_test(){
	TEST_HEADER;

	int a = 10;
	int b = 0;
	int c = a/b;
	return c;
}

/* Page Fault Before Kernel Test - Example
 * 
 * Asserts that you cannot access memory before kernel
 * Inputs: None
 * Outputs: None
 * Side Effects: throws exception message and freezes OS
 * Coverage: Exception Handling/Paging
 */
int page_fault_before_kernel(){

	TEST_HEADER;
	int* ptr;
	int other_ptr;
	for(ptr = (int*)(0x300000); ptr< (int*)(0x400000);ptr++){
		other_ptr = *(ptr);
	}
	return FAIL;
}

/* Page Fault After Kernel Test - Example
 * 
 * Asserts that you cannot access memory before kernel
 * Inputs: None
 * Outputs: None
 * Side Effects: throws exception message and freezes OS
 * Coverage: Exception Handling/Paging
 */
int page_fault_after_kernel(){

	TEST_HEADER;
	int* ptr;
	int other_ptr;
	for(ptr = (int*)(0x800000); ptr<= (int*)(0x900000);ptr++){
		other_ptr = *(ptr);
	}
	return FAIL;
}

/* Page is Within Kernel Test - Example
 * 
 * Asserts that you can access memory within kernel
 * Inputs: None
 * Outputs: None
 * Side Effects: throws message at you that page is within kernel memory
 * Coverage: Exception Handling/Paging
 */
int page_is_within_kernel(){
	TEST_HEADER;
	int* ptr;
	int other_ptr;
	for(ptr = (int*)(0x400000); ptr< (int*)(0x800000);ptr++){
		other_ptr = *(ptr);
	}
	printf("page is within kernel");
	return PASS;
}

/* Page Fault Before Video Test - Example
 * 
 * Asserts that you cannot access before video memory
 * Inputs: None
 * Outputs: None
 * Side Effects: throws exception message and freezes OS
 * Coverage: Exception Handling/Paging
 */
int page_fault_before_video(){

	TEST_HEADER;
	int* ptr;
	int other_ptr;
	for(ptr = (int*)(0xB7000); ptr< (int*)(0xB8000);ptr++){
		other_ptr = *(ptr);
	}
	return FAIL;
}

/* Page is Within Video Test - Example
 * 
 * Asserts that you can access memory within video
 * Inputs: None
 * Outputs: None
 * Side Effects: throws message at you that page is within video memory
 * Coverage: Exception Handling/Paging
 */
int page_is_within_video(){
	TEST_HEADER;
	int* ptr;
	int other_ptr;
	for(ptr = (int*)(0xB8000); ptr< (int*)(0xB9000);ptr++){
		other_ptr = *(ptr);
	}
	printf("page is within video memory");
	return PASS;
}


/* Page Fault After Video Test - Example
 * 
 * Asserts that you cannot access after video memory
 * Inputs: None
 * Outputs: None
 * Side Effects: throws exception message and freezes OS
 * Coverage: Exception Handling/Paging
 */
int page_fault_after_video(){

	TEST_HEADER;
	int* ptr;
	int other_ptr;
	for(ptr = (int*)(0xB9000); ptr<= (int*)(0xC0000);ptr++){
		other_ptr = *(ptr);
	}
	return FAIL;
}


// add more tests here

/* Checkpoint 2 tests */


/* Test RTC Freq - Example
 * 
 * Checks if rate is being doubled for increasing frequency
 * Inputs: None
 * Outputs: Prints 1s at speed of respective rate
 * Side Effects: None
 * Coverage: RTC 
 */
int test_rtc_freq(){
    TEST_HEADER;
    int i,j;  
    for(i= 2; i <= 1024; i*=2){
       
        printf(" %d Hz:    ", i);

        rtc_read();

        rtc_write(NULL, (void*)i, NULL);
       
        for(j = 0; j < 15; j++){
            printf("1");
            rtc_read();

        }
       
        printf("\n");
    }
    return PASS;
}


/* Test RTC Open - Example
 * 
 * Checks if RTC opens
 * Inputs: None
 * Outputs: PASS or FAIL 
 * Side Effects: None
 * Coverage: RTC 
 */
int test_rtc_open(){
    TEST_HEADER;
    int i;

    printf("Initial Freq is 1024 before, printing 30 chars at this freq : \n");

    for(i = 0; i< 30; i++){
        printf("1");
        rtc_read();
    }
    printf("\n\n");

    rtc_open();
    printf("New Freq is 2 after calling rtc_open, printing 15 chars at this freq : \n");

    for(i = 0; i< 15; i++){
        printf("1");
        rtc_read();
    }

    printf("\n\n");
    return PASS;
}


/* Test RTC Close - Example
 * 
 * Checks if RTC closes
 * Inputs: None
 * Outputs: PASS or FAIL 
 * Side Effects: None
 * Coverage: RTC 
 */
int test_rtc_close(){
	TEST_HEADER;
    printf(" PASS if rtc_close returns 0, else FAIL \n \n");
    if(rtc_close() == 0){
        return PASS;
    }
    return FAIL;

}


/* Test Terminal Open - Example
 * 
 * Checks if terminal opens
 * Inputs: None
 * Outputs: PASS or FAIL 
 * Side Effects: None
 * Coverage: Terminal 
 */
int terminal_open_test()
{
	TEST_HEADER;
	uint8_t filename;
	int result = PASS;
	if (terminal_open(&filename) == -1){
		result = FAIL;
		assertion_failure();
		printf("fail");
	}
	printf("success");
	return result;
}


/* Test Terminal Close - Example
 * 
 * Checks if terminal closes
 * Inputs: None
 * Outputs: PASS or FAIL 
 * Side Effects: None
 * Coverage: Terminal 
 */
int terminal_close_test()
{
	TEST_HEADER;
	uint32_t fd = 3;
	int result = PASS;
	if (terminal_close(fd) != -1){
		result = FAIL;
		assertion_failure();
		printf("fail");
	}
	printf("success");
	return result;
}

/* Test Terminal Write - Example
 * 
 * Checks if terminal writing memory
 * Inputs: None
 * Outputs: PASS or FAIL 
 * Side Effects: None
 * Coverage: Terminal 
 */
int terminal_write_test()
{
	TEST_HEADER;
	uint32_t fd = 3;
	char test_write[] = "This is a test for terminal write function!";
	int result = PASS;
	if (terminal_write(fd, test_write, strlen(test_write)) != strlen(test_write)){
		result = FAIL;
		assertion_failure();
		printf("fail");
	}

	printf("success");
	return result;
}

/* Test Terminal Read - Example
 * 
 * Checks if terminal reading memory
 * Inputs: None
 * Outputs: PASS or FAIL 
 * Side Effects: None
 * Coverage: Terminal 
 */
int terminal_read_test()
{
	TEST_HEADER;
	uint32_t fd = 0;
	int32_t nbytes;
	char test_buf[128];
	//uint8_t filename_;
	//terminal_open(filename_);
	terminal_write(fd, "testing size 10\n", 16);
	nbytes = terminal_read(fd, test_buf, 10);
	printf("size 10 test\n%d\n", nbytes);
	terminal_write(fd, test_buf, nbytes);
	//terminal_close(fd);

	printf("\n");

	//terminal_open(filename_);
	terminal_write(fd, "what's your favorite food\n", 27);
	nbytes = terminal_read(fd, test_buf, 128);
	printf("Ok I know it's ");
	terminal_write(fd, test_buf, nbytes);
	//terminal_close(fd);
	//nbytes = terminal_read(fd, test_buf, 126);
	//printf("size 126 test\n%d\n", nbytes);

	//terminal_write(fd, "testing size 128\n", 17);
	/*nbytes = terminal_read(fd, test_buf, 128);
	printf("size 128 test\n%d\n", nbytes);

	nbytes = terminal_read(fd, test_buf, 129);
	printf("size 129 test\n%d\n", nbytes);*/


	return PASS;
}



/* Read Files Test
 * 
 * Checks if DirEntry reading works
 * Inputs: None
 * Outputs: PASS, you can tell if it works based on if it prints the expected value
 * Side Effects: None
 * Coverage: DirEntry Read 
 */
int read_files_test(){
	TEST_HEADER;
	int i, length;
	uint8_t buffer[63][FILENAMESIZE];
	dir_entry_t * beingTested;
	for(i=0; i<63; i++){
		
		length = dirEntryRead(0, buffer[i], FILENAMESIZE);


		if(length > 0){
			read_dirEntry_by_name(buffer[i], beingTested);
			printf("file_name: ");
			printf((int8_t*)buffer[i]);
			printf(", file_type: %d", beingTested->FILETYPE);
			printf(", file_size: %d", (inodeHead + (beingTested->INODENUM))->LENGTH);
			printf("\n");
		}

		

	}

	return PASS;
}


/* Read Files Data Test
 * 
 * Checks if reading data from a file works
 * Inputs: None
 * Outputs: PASS, you can tell if it works based on if it prints the expected value (frame0.txt printed out correctly)
 * Side Effects: None
 * Coverage: Data Read 
 */
int read_file_data_test(){

	TEST_HEADER;

	int i, length, j, readBits, filesRead;
	uint8_t buffer[63][FILENAMESIZE];
	uint8_t bufferFile[63][40000];

	filesRead = 0;
	for(i=0; i<63; i++){
		
		length = dirEntryRead(0, buffer[i], FILENAMESIZE);


		if(length > 0){

			readBits = fileRead(0, bufferFile[i], 40000);

			if(readBits > 0) {
				filesRead++;

				if(filesRead == 9) {

					for(j = 0; j < readBits; j++) {

						putc(bufferFile[i][j]);

					}

					break;

				}

				

			}

		}

		

	}

	return PASS;

}

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	// TEST_OUTPUT("idt_test", idt_test());
	//terminal_read_test();
	// TEST_OUTPUT("terminal_write_test", terminal_write_test());
	// TEST_OUTPUT("terminal_open_test", terminal_open_test());
	// TEST_OUTPUT("terminal_close_test", terminal_close_test());
	// TEST_OUTPUT("dividebyzero_test", dividebyzero_test());
	// page_fault_before_kernel();
	// page_fault_after_kernel();
	// page_is_within_kernel();
	// page_fault_before_video();
	// page_is_within_video();
	// page_fault_after_video();
	//TEST_OUTPUT("test_rtc_close", test_rtc_close());
	//TEST_OUTPUT("test_rtc_open", test_rtc_open());
	//TEST_OUTPUT("test_rtc_freq", test_rtc_freq());
	//TEST_OUTPUT("read_files_test", read_files_test());
	//TEST_OUTPUT("read_file_data_test", read_file_data_test());
	// launch your tests here
}


