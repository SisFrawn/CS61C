#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <CUnit/Basic.h>
#include "beargit.h"
#include "util.h"

/* printf/fprintf calls in this tester will NOT go to file. */

#undef printf
#undef fprintf

/* The suite initialization function.
 * You'll probably want to delete any leftover files in .beargit from previous
 * tests, along with the .beargit directory itself.
 *
 * You'll most likely be able to share this across suites.
 */
int init_suite(void)
{
    // preps to run tests by deleting the .beargit directory if it exists
    fs_force_rm_beargit_dir();
    unlink("TEST_STDOUT");
    unlink("TEST_STDERR");
    return 0;
}

/* You can also delete leftover files after a test suite runs, but there's
 * no need to duplicate code between this and init_suite
 */
int clean_suite(void)
{
    return 0;
}

struct commit {
    char msg[MSG_SIZE];
    struct commit* next;
};


void free_commit_list(struct commit** commit_list) {
    if (*commit_list) {
        free_commit_list(&((*commit_list)->next));
        free(*commit_list);
    }
    
    *commit_list = NULL;
}

void run_commit(struct commit** commit_list, const char* msg) {
    int retval = beargit_commit(msg);
    CU_ASSERT(0==retval);
    
    struct commit* new_commit = (struct commit*)malloc(sizeof(struct commit));
    new_commit->next = *commit_list;
    strcpy(new_commit->msg, msg);
    *commit_list = new_commit;
}

void simple_sample_test(void)
{
    // This is a very basic test. Your tests should likely do more than this.
    // We suggest checking the outputs of printfs/fprintfs to both stdout
    // and stderr. To make this convenient for you, the tester replaces
    // printf and fprintf with copies that write data to a file for you
    // to access. To access all output written to stdout, you can read
    // from the "TEST_STDOUT" file. To access all output written to stderr,
    // you can read from the "TEST_STDERR" file.
    int retval;
    retval = beargit_init();
    CU_ASSERT(0==retval);
    retval = beargit_add("asdf.txt");
    CU_ASSERT(0==retval);
}

void simple_log_test(void)
{
    //Ensuring proper initialization
    int retval;
    retval = beargit_init();
    CU_ASSERT(0==retval);
    
    //Creating a file
    FILE* asdf = fopen("asdf.txt", "w");
    fclose(asdf);
    
    //Adding it and ensuring it was properly added
    retval = beargit_add("asdf.txt");
    CU_ASSERT(0==retval);
    
    //3 commits
    struct commit* commit_list = NULL;
    run_commit(&commit_list, "THIS IS BEAR TERRITORY!1");
    run_commit(&commit_list, "THIS IS BEAR TERRITORY!2");
    run_commit(&commit_list, "THIS IS BEAR TERRITORY!3");
    
    //No errors while running log
    retval = beargit_log(10);
    CU_ASSERT(0==retval);
    
    //Commit linked list pointer
    struct commit* cur_commit = commit_list;
    
    //Setup for redirecting stdout
    const int LINE_SIZE = 512;
    char line[LINE_SIZE];
    
    FILE* fstdout = fopen("TEST_STDOUT", "r");
    CU_ASSERT_PTR_NOT_NULL(fstdout);
    
    while (cur_commit != NULL) {
        char refline[LINE_SIZE];
        
        // First line is commit -- don't check the ID.
        CU_ASSERT_PTR_NOT_NULL(fgets(line, LINE_SIZE, fstdout));
        CU_ASSERT(!strncmp(line,"commit", strlen("commit")));
        
        // Second line is msg
        sprintf(refline, "   %s\n", cur_commit->msg);
        CU_ASSERT_PTR_NOT_NULL(fgets(line, LINE_SIZE, fstdout));
        CU_ASSERT_STRING_EQUAL(line, refline);
        
        // Third line is empty
        CU_ASSERT_PTR_NOT_NULL(fgets(line, LINE_SIZE, fstdout));
        CU_ASSERT(!strcmp(line,"\n"));
        
        cur_commit = cur_commit->next;
    }
    
    CU_ASSERT_PTR_NULL(fgets(line, LINE_SIZE, fstdout));
    
    // It's the end of output
    CU_ASSERT(feof(fstdout));
    fclose(fstdout);
    
    free_commit_list(&commit_list);
}


/**
 * @author Kunal Mishra
 *
 * Beargit methods used/tested:
 *      add
 *      remove
 *      status      <-- priority
 *      commit      <-- priority #2
 *
 * Purpose: This test is designed to ensure beargit is properly tracking files across commits adds and removes
 */
void statusAndCommitTest() {
    //Ensuring proper initialization
    int retval;
    retval = beargit_init();
    CU_ASSERT(0==retval);
    
    //Creating a file
    FILE* kunal = fopen("kunal.txt", "w");
    fclose(kunal);
    
    //Adding it and ensuring it was properly added
    retval = beargit_add("kunal.txt");
    CU_ASSERT(0==retval);
    
    //Checking that status displays tracked files properly
    retval = beargit_status();
    CU_ASSERT(0==retval);
    
    //Setup for redirecting stdout of status
    const int LINE_SIZE = 512;
    char line[LINE_SIZE];
    
    FILE* fstdout = fopen("TEST_STDOUT", "r");
    CU_ASSERT_PTR_NOT_NULL(fstdout);
    
    //Reading stdout
    int count = 0;
    while (fgets(line, LINE_SIZE, fstdout))             count += 1;
    
    //First line is added file (should only be one thing in stdout)
    CU_ASSERT(5==count);
    
    fclose(fstdout);
    
    //Setup for commit
    struct commit* commit_list = NULL;
    char* firstCommitMsg= "THIS IS BEAR TERRITORY!1--Committed Kunal";
    run_commit(&commit_list, firstCommitMsg);
    
    //Creating a second file
    FILE* tony = fopen("tony.txt", "w");
    fclose(tony);
    
    //Adding it and ensuring it was properly added
    retval = beargit_add("tony.txt");
    CU_ASSERT(0==retval);
    
    //Getting ready to read in from stdout
    fstdout = fopen("TEST_STDOUT", "r");
    CU_ASSERT_PTR_NOT_NULL(fstdout);
    
    //Index should now be tracking two files (one from before, one from recent add)
    count = 0;
    while (fgets(line, LINE_SIZE, fstdout)) {
        if (count == 2)         CU_ASSERT(!strncmp(line,"kunal.txt", strlen("kunal.txt")));         //First added file
        if (count == 3)         CU_ASSERT(!strncmp(line,"tony.txt", strlen("tony.txt")));           //Second added file
        count += 1;
    }
    
    fclose(fstdout);
    
    //There should only be two things in status
    CU_ASSERT(6==count);
    
    //Removing the first file
    retval = beargit_rm("kunal.txt");
    CU_ASSERT(0==retval);
    
    //Committing changes
    char* secondCommitMsg= "THIS IS BEAR TERRITORY!2--Added Tony, removed Kunal";
    run_commit(&commit_list, secondCommitMsg);
    
    //Checking that status displays tracked files properly
    retval = beargit_status();
    CU_ASSERT(0==retval);
    
    //Getting ready to read in from stdout
    fstdout = fopen("TEST_STDOUT", "r");
    CU_ASSERT_PTR_NOT_NULL(fstdout);
    
    //Index should now be tracking one file (tony)
    count = 0;
    while (fgets(line, LINE_SIZE, fstdout))         count += 1;
    
    //There should only be one thing in status
    CU_ASSERT(5==count);
    
    fclose(fstdout);
}

/**
 * @author Kunal Mishra
 *
 * Beargit methods used/tested:
 *      add
 *      remove
 *      status
 *      checkout    <-- priority
 *      commit      <-- priority #2
 *
 * Purpose: This test is designed to ensure commit and checkout are working properly together
 * A slightly simpler version of a unix bash file we wrote to test outside of the cunit framework
 
 
 echo "XXXXXXXXXXXXXXXXXXXXXX START TEST XXXXXXXXXXXXXXXXXXXXX"
 echo ""
 echo ""
 echo ""
 
 beargit init
 
 touch file1.txt
 touch file2.txt
 touch file3.txt
 
 echo "===> checkout a branch before any commit is made"
 beargit add file1.txt
 beargit checkout master
 
 echo "===> below should not display file1.txt"
 beargit status
 touch file1.txt
 beargit add file1.txt
 
 echo "===> below should now display file1.txt"
 beargit status
 beargit commit -m "THIS IS BEAR TERRITORY! THIS IS BEAR TERRITORY!"
 
 echo "===> commit should go through"
 beargit log
 beargit checkout 5fe5991ffba74e3d74a71939068a32bcc4605121
 
 echo "===> master should not have a star next to it"
 beargit branch
 
 echo "===> commit should not go through"
 beargit commit -m "THIS IS BEAR TERRITORY! THIS SHOULD NOT GO THROUGH!"
 
 #Kunal adding:
 echo "===> checking out to a new branch, no errors expected"
 beargit checkout -b "new_branch"
 
 echo "===> commit should go through now (at the head of a branch)"
 beargit commit -m "THIS IS BEAR TERRITORY! NEW BRANCH";
 
 echo "===> status should now track file2.txt in addition to file1.txt"
 beargit add file2.txt
 beargit status
 
 echo "===> commit should go through"
 beargit commit -m "THIS IS BEAR TERRITORY! ADDED FILE 2"
 
 echo "===> checking out to master... should not be tracking file2.txt"
 beargit checkout master
 beargit status
 
 echo "===> creating a new branch with the same head as master (no -b flag so error should occur)"
 beargit checkout master2
 
 echo "===> should work now, no error expected"
 beargit checkout -b master2
 
 echo "===> adding file3 and removing file1"
 beargit add file3.txt
 beargit rm file1.txt
 beargit status
 
 echo "===> commit should go through"
 beargit commit -m "THIS IS BEAR TERRITORY! SHOULD BE 1 COMMIT AHEAD OF MASTER, DIFFERENT BRANCH"
 
 echo "===> showing logs for master2 and master (master2 should be one commit longer or \"ahead\" of master)"
 beargit log
 
 echo "===> Master:"
 beargit checkout master
 beargit log
 
 echo "===> Master should still be tracking only file1"
 beargit status
 
 echo "===> END TEST"
 */
void commitAndCheckoutTest() {
    //Ensuring proper initialization
    int retval;
    retval = beargit_init();
    CU_ASSERT(0==retval);
    
    //Creating files
    FILE* file1 = fopen("file1", "w");
    FILE* file2 = fopen("file2", "w");
    FILE* file3 = fopen("file3", "w");
    fclose(file1);
    fclose(file2);
    fclose(file3);
    
    //Checkout before committing anything
    retval = beargit_add("file1");
    CU_ASSERT(0==retval);
    retval = beargit_checkout("master", 0);
    CU_ASSERT(0==retval);
    
    //Status should not print anything out (added file was not saved)
    retval = beargit_status();
    CU_ASSERT(0==retval);
    
    //Setup for redirecting stdout of status
    const int LINE_SIZE = 512;
    char line[LINE_SIZE];
    
    FILE* fstdout = fopen("TEST_STDOUT", "r");
    CU_ASSERT_PTR_NOT_NULL(fstdout);
    
    //Reading stdout
    int count = 0;
    while (fgets(line, LINE_SIZE, fstdout))             count += 1;
    
    //Nothing should be in status
    CU_ASSERT(4==count);
    
    fclose(fstdout);

    //Adding for real now
    retval = beargit_add("file1");
    CU_ASSERT(0==retval);
    
    //Setup for commit
    struct commit* commit_list = NULL;
    char* firstCommitMsg= "THIS IS BEAR TERRITORY!1--Committed Kunal";
    
    //Committing changes
    run_commit(&commit_list, firstCommitMsg);
    
    //Detaching from master
    retval = beargit_checkout("5fe5991ffba74e3d74a71939068a32bcc4605121", 0);
    CU_ASSERT(0==retval);
    
    //Trying to commit again (shouldn't be able to)
    char * secondCommitMsg = "THIS IS BEAR TERRITORY!2";
    retval = beargit_commit(secondCommitMsg);
    CU_ASSERT(1==retval);
    
    //Trying to checkout to a new branch, no errors expected
    retval = beargit_checkout("new_branch", 1);
    CU_ASSERT(0==retval);
    
    //Trying to commit again (should be able to now)
    run_commit(&commit_list, secondCommitMsg);
    
    //Adding file2
    retval = beargit_add("file2");
    CU_ASSERT(0==retval);
    
    //Status should now be tracking both file1 and file2
    retval = beargit_status();
    CU_ASSERT(0==retval);
    
    fstdout = fopen("TEST_STDOUT", "r");
    CU_ASSERT_PTR_NOT_NULL(fstdout);
    
    //Reading stdout
    count = 0;
    while (fgets(line, LINE_SIZE, fstdout))             count += 1;
    
    //There should be 2 files tracked (6 lines)
    CU_ASSERT(6==count);
    
    fclose(fstdout);
    
    //Committing changes (additions) to new_branch
    char * thirdCommitMsg = "THIS IS BEAR TERRITORY!3";
    run_commit(&commit_list, thirdCommitMsg);
    
    //Checking back out to master
    beargit_checkout("master", 0);
    
    //Master should only be tracking file1
    retval = beargit_status();
    CU_ASSERT(0==retval);
    
    fstdout = fopen("TEST_STDOUT", "r");
    CU_ASSERT_PTR_NOT_NULL(fstdout);
    
    //Reading stdout
    count = 0;
    while (fgets(line, LINE_SIZE, fstdout))             count += 1;
    
    //There should be 1 file tracked (5 lines)
    CU_ASSERT(5==count);
    
    fclose(fstdout);
}


/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 */
int cunittester()
{
    //Initializing suites of tests
    CU_pSuite pSuite = NULL;
    CU_pSuite pSuite2 = NULL;
    
    CU_pSuite mySuite1 = NULL;
    CU_pSuite mySuite2 = NULL;
    
    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();
    
    /* add a suite to the registry */
    pSuite = CU_add_suite("Suite_1", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    
    /* Add tests to the Suite #1 */
    if (NULL == CU_add_test(pSuite, "Simple Test #1", simple_sample_test))
    {
        CU_cleanup_registry();
        return CU_get_error();
    }
    
    pSuite2 = CU_add_suite("Suite_2", init_suite, clean_suite);
    if (NULL == pSuite2) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    
    /* Add tests to the Suite #2 */
    if (NULL == CU_add_test(pSuite2, "Log output test", simple_log_test))
    {
        CU_cleanup_registry();
        return CU_get_error();
    }
    
    
    /* add a suite to the registry */
    mySuite1 = CU_add_suite("Suite_3", init_suite, clean_suite);
    if (NULL == mySuite1) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    
    /* Add tests to the Suite #3 */
    if (NULL == CU_add_test(mySuite1, "Status and Commit", statusAndCommitTest))
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add a suite to the registry */
    mySuite2 = CU_add_suite("Suite_4", init_suite, clean_suite);
    if (NULL == mySuite2) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    
    /* Add tests to the Suite #4 */
    if (NULL == CU_add_test(mySuite2, "Checkout and Commit", commitAndCheckoutTest))
    {
        CU_cleanup_registry();
        return CU_get_error();
    }
    
    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}

