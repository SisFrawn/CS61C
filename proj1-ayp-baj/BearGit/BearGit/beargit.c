#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>

#include "beargit.h"
#include "util.h"

/* Implementation Notes:
 *
 * - Functions return 0 if successful, 1 if there is an error.
 * - All error conditions in the function description need to be implemented
 *   and written to stderr. We catch some additional errors for you in main.c.
 * - Output to stdout needs to be exactly as specified in the function description.
 * - Only edit this file (beargit.c)
 * - Here are some of the helper functions from util.h:
 *   * fs_mkdir(dirname): create directory <dirname>
 *   * fs_rm(filename): delete file <filename>
 *   * fs_mv(src,dst): move file <src> to <dst>, overwriting <dst> if it exists
 *   * fs_cp(src,dst): copy file <src> to <dst>, overwriting <dst> if it exists
 *   * write_string_to_file(filename,str): write <str> to filename (overwriting contents)
 *   * read_string_from_file(filename,str,size): read a string of at most <size> (incl.
 *     NULL character) from file <filename> and store it into <str>. Note that <str>
 *     needs to be large enough to hold that string.
 *  - You NEED to test your code. The autograder we provide does not contain the
 *    full set of tests that we will run on your code. See "Step 5" in the project spec.
 */

/* beargit init
 *
 * - Create .beargit directory
 * - Create empty .beargit/.index file
 * - Create .beargit/.prev file containing 0..0 commit id
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_init(void) {
    //Make a new hidden beargit directory
    fs_mkdir(".beargit");
    
    //Create a .index file within beargit
    FILE* findex = fopen(".beargit/.index", "w");
    fclose(findex);
    
    //Initialize a branches file within beargit
    FILE* fbranches = fopen(".beargit/.branches", "w");
    fprintf(fbranches, "%s\n", "master");
    fclose(fbranches);
    
    //Initialize a previous commit file and a current branch file within beargit
    write_string_to_file(".beargit/.prev", "0000000000000000000000000000000000000000");
    write_string_to_file(".beargit/.current_branch", "master");
    
    return 0;
}


/* beargit add <filename>
 *
 * - Append filename to list in .beargit/.index if it isn't in there yet
 *
 * Possible errors (to stderr):
 * >> ERROR:  File <filename> has already been added.
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_add(const char* filename) {
    //Opening the current index file and creating a new index file
    FILE* findex = fopen(".beargit/.index", "r");
    FILE* fnewindex = fopen(".beargit/.newindex", "w");
    
    //Reads tracked files in from the current index and places them in the newIndex
    char line[FILENAME_SIZE];
    while(fgets(line, sizeof(line), findex)) {
        strtok(line, "\n");
        //Checks to make sure file isn't already tracked
        if (strcmp(line, filename) == 0) {
            fprintf(stderr, "ERROR:  File %s has already been added.\n", filename);
            fclose(findex);
            fclose(fnewindex);
            fs_rm(".beargit/.newindex");
            return 3;
        }
        //Copies over tracked files
        fprintf(fnewindex, "%s\n", line);
    }
    
    //Places the newly tracked file in the new index
    fprintf(fnewindex, "%s\n", filename);
    fclose(findex);
    fclose(fnewindex);
    
    //Overwrites the old file with the updated index
    fs_mv(".beargit/.newindex", ".beargit/.index");
    
    return 0;
}

/* beargit status                       NOT YET TESTED
 *
 * See "Step 1" in the project spec.
 *
 */

int beargit_status() {
    //Open the current index
    FILE* findex = fopen(".beargit/.index", "r");
    
    //Reads tracked files in from the index and prints them out
    char line[FILENAME_SIZE];
    int count = 0;
    
    printf("Tracked files:\n\n");
    
    while(fgets(line, sizeof(line), findex)) {
        strtok(line, "\n");

        //Prints each tracked file
        printf("%s\n", line);
        
        //The number of tracked files
        count += 1;
    }
    
    //Total tracked files
    printf("\nThere are %d files total.\n", count);
    
    //Cleaning up
    fclose(findex);
    
    return 0;
}

/* beargit rm <filename>                NOT YET TESTED
 *
 *
 * See "Step 2" in the project spec.
 *
 */

int beargit_rm(const char* filename) {
    //Opening the current index file and creating a new index file
    FILE* findex = fopen(".beargit/.index", "r");
    FILE* fnewindex = fopen(".beargit/.newindex", "w");
    
    //found is a boolean that tracks whether the file to remove was found or not
    int found = 0;
    
    //Reads tracked files in from the current index and places them in the newIndex unless they're being removed
    char line[FILENAME_SIZE];
    while(fgets(line, sizeof(line), findex)) {
        strtok(line, "\n");
        
        //Finding the file among tracked files in index
        if (strcmp(line, filename) == 0) {
            found = 1;
        
        //If it wasn't supposed to be removed, keep tracking it by copying to the new index
        } else {
            fprintf(fnewindex, "%s\n", line);
        }
    }
    
    //Cleaning up (if the file to be removed wasn't found, its just a copy of the original anyway)
    fclose(findex);
    fclose(fnewindex);
    fs_mv(".beargit/.newindex", ".beargit/.index");
    
    //Error if it wasn't found
    if (found)      return 0;
    else {
        fprintf(stderr, "ERROR:  File %s not tracked.\n", filename);
        return 1;
    }
}

/* beargit commit -m <msg>
 *
 * See "Step 3" in the project spec.
 *
 */

const char* go_bears = "THIS IS BEAR TERRITORY!";

//Returns 1 if string contains substring and 0 if it does not
int containsString(char* substring, char* string) {
    char * pointer = substring;
    char * pointer2 = string;
    
    while (*pointer) {
        //If they're not equal, the substring is not contained in the string
        if (*pointer != *pointer2)                        return 0;
        //Advance both pointers
        pointer++; pointer2++;
        //If the string has run out of space and the substring has not, it is not contained in the string
        if ((pointer2 == NULL) && (pointer != NULL))    return 0;
    }
    
    return 1;
}

int is_commit_msg_ok(const char* msg) {
    char* pointer = msg;
    
    while (*pointer) {
        if (containsString(go_bears, pointer))      return 1;
        pointer++;
    }
    
    
    return 0;
}

/* Use next_commit_id to fill in the rest of the commit ID.
 *
 * Hints:
 * You will need a destination string buffer to hold your next_commit_id, before you copy it back to commit_id
 * You will need to use a function we have provided for you.
 */

void next_commit_id(char* commit_id) {
    //Getting the current branch
    char branch[BRANCHNAME_SIZE];
    read_string_from_file(".beargit/.branches", branch, BRANCHNAME_SIZE);
    
    //Combining the current branch and previous commit ID into combination array
    char combination[BRANCHNAME_SIZE + COMMIT_ID_SIZE + 1];
    
    //Combining into one string
    strcpy(combination, branch);
    strcat(combination, commit_id);
    
    //Using cryptohash on combination string and reassigning commit_id
    cryptohash(combination, commit_id);
}

void copyIntoDirectoryWithExtension(char * dirPath, char * extension) {
    //Combine the dirPath and extension
    char dirPathCombination[FILENAME_SIZE];
    strcpy(dirPathCombination, dirPath);
    strcat(dirPathCombination, "/");
    strcat(dirPathCombination, extension);
    
    
    //Copy the file into the .beargit directory
    fs_cp(extension, dirPathCombination);
}

int beargit_commit(const char* msg) {
    // check whether at head of a branch
    char current_branch[BRANCHNAME_SIZE];
    read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);
    
    if (!strlen(current_branch)) {
        fprintf(stderr, "ERROR:  Need to be on HEAD of a branch to commit.\n");
        return 1;
    }
    
    //Checking the valid commit msg was supplied
    if (!is_commit_msg_ok(msg)) {
        fprintf(stderr, "ERROR:  Message must contain \"%s\"\n", go_bears);
        return 1;
    }
    
    //Generating a new commit id
    char commit_id[COMMIT_ID_SIZE];
    read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);
    next_commit_id(commit_id);
    
    
    //Generate a new directory
    char dirPath[FILENAME_SIZE];
    strcpy(dirPath, ".beargit/");                               //dirpath: ".beargit/"
    strcat(dirPath, commit_id);                                 //dirpath: ".beargit/commit_id"
    fs_mkdir(dirPath);
    
    
    //Copying over prev
    char prevDestination[FILENAME_SIZE];
    strcpy(prevDestination, dirPath);
    strcat(prevDestination, "/.prev");                          //prevDestination: dirpath + "/.prev"
    fs_cp(".beargit/.prev", prevDestination);
    
    
    //Copying over index
    char indexDestination[FILENAME_SIZE];
    strcpy(indexDestination, dirPath);
    strcat(indexDestination, "/.index");                        //indexDestination: dirpath+ "/.index"
    fs_cp(".beargit/.index", indexDestination);
    
    
    //Create a .index file instance to read within beargit
    FILE* findex = fopen(".beargit/.index", "r");
    
    //Reads tracked files in from the current index and places them in the newdir
    char line[FILENAME_SIZE];
    while(fgets(line, sizeof(line), findex)) {
        strtok(line, "\n");
        
        //Copying dirpath and concatenating the name of the tracked file
        copyIntoDirectoryWithExtension(dirPath, line);
    }
    
    //Cleaning up
    fclose(findex);
    
    //Storing commit msg
    char msgDestination[FILENAME_SIZE];
    strcpy(msgDestination, dirPath);
    strcat(msgDestination, "/.msg");                                //msgDestination: dirpath + "/.msg"
    
    FILE* msgFile = fopen(msgDestination, "w");
    fprintf(msgFile, "%s\n", msg);
    fclose(msgFile);
    
    //Updating prev
    write_string_to_file(".beargit/.prev", commit_id);
    
    return 0;
}




/* beargit log
 *
 * See "Step 4" in the project spec.
 *
 */

//Prints out information for commit for use in log and returns a pointer to the prev commit ID
char* commitLog(char* commitID) {
    //Getting the directory name of the commit
    char dirpath[COMMIT_ID_SIZE + strlen(".beargit/")];
    strcpy(dirpath, ".beargit/");
    strcat(dirpath, commitID);
    
    //Get the msg from that commit
    char msgDst[MSG_SIZE];
    char msgSrc[strlen(dirpath) + strlen("/.msg")];
    
    strcpy(msgSrc, dirpath);
    strcat(msgSrc, "/.msg");
    read_string_from_file(msgSrc, msgDst, MSG_SIZE);
    
    strtok(msgDst, "\n");
    
    //Print out things necessary for the commit's log
    printf("commit %s\n", commitID);
    printf("   %s\n\n", msgDst);
    
    //Get the previous commitID
    char prevCommitID[COMMIT_ID_SIZE];
    char prevSrc[strlen(dirpath) + strlen("/.prev")];
    
    strcpy(prevSrc, dirpath);
    strcat(prevSrc, "/.prev");
    read_string_from_file(prevSrc, prevCommitID, COMMIT_ID_SIZE);
    
    //Return the previous commit ID
    return prevCommitID;
}

int beargit_log(int limit) {
    //Reading in previous commit ID
    char prevCommit[COMMIT_ID_SIZE];
    char* prevCommitPtr = prevCommit;
    read_string_from_file(".beargit/.prev", prevCommit, COMMIT_ID_SIZE);
    
    char* end = "0000000000000000000000000000000000000000";
    
    for (int i = 0; i < limit; i+=1) {
        
        //Base case for stopping loop
        if (strcmp(end, prevCommitPtr) == 0) {
            //There are no previous commits
            if (i == 0) {
                fprintf(stderr, "ERROR:  There are no commits.\n");
                return 1;
            }
            break;
        }
        
        //Advances commit to the next prevCommit
        prevCommitPtr = commitLog(prevCommitPtr);
    }
    
    
    return 0;
}


// This helper function returns the branch number for a specific branch, or
// returns -1 if the branch does not exist.
int get_branch_number(const char* branch_name) {
    FILE* fbranches = fopen(".beargit/.branches", "r");
    
    int branch_index = -1;
    int counter = 0;
    char line[FILENAME_SIZE];
    while(fgets(line, sizeof(line), fbranches)) {
        strtok(line, "\n");
        if (strcmp(line, branch_name) == 0) {
            branch_index = counter;
        }
        counter++;
    }
    
    fclose(fbranches);
    
    return branch_index;
}

/* beargit branch
 *
 * See "Step 5" in the project spec.
 *
 */

int beargit_branch() {
    FILE* fbranches = fopen(".beargit/.branches", "r");
    FILE* fcurrbranch = fopen(".beargit/.current_branch", "r");
    
    char branch_name[BRANCHNAME_SIZE];
    read_string_from_file(".beargit/.current_branch", branch_name, BRANCHNAME_SIZE);
    strtok(branch_name, "\n");
    
    char line[FILENAME_SIZE];
    while(fgets(line, sizeof(line), fbranches)) {
        strtok(line, "\n");
        
        if (strcmp(line, branch_name) == 0)     printf("*");
        else                                    printf(" ");
        
        printf("  ");
        printf(line);
        printf("\n");
    }
    
    fclose(fbranches);
    
    return 0;
}

/* beargit checkout
 *
 * See "Step 6" in the project spec.
 *
 */

int checkout_commit(const char* commit_id) {
    const char* beargit = ".beargit/";
    const char* index = ".index";
    const char* dash = "/";
    
    char line[FILENAME_SIZE];
    char file[FILENAME_SIZE];
    FILE* f_index;
    
    // Going through the index of the current index file,
    // delete all those files (in the current directory;
    // i.e., the directory where we ran beargit).
    char current_index[FILENAME_SIZE] = ".beargit/.index";
    
    f_index = fopen(current_index, "r");
    
    while(fgets(line, sizeof(line), f_index)) {
        strtok(line, "\n");
        fs_rm(line);
    }
    
    fclose(f_index);
    
    // write the id of the commit that is being checked out into .prev
    write_string_to_file(".beargit/.prev", commit_id);
    
    // remove the current index
    fs_rm(current_index);
    FILE* findex = fopen(".beargit/.index", "w");
    fclose(findex);
    
    if (0 == strcmp("0000000000000000000000000000000000000000", commit_id)) {
        return 0;
    }
    
    // Copy the index from the commit that is being
    // checked out to the .beargit directory,
    // and use it to copy all that commit's tracked
    // files from the commit's directory into the current directory
    char commit_path[FILENAME_SIZE]; // ./beargit/<commit_id>
    strcpy(commit_path, beargit);
    strcat(commit_path, dash);
    strcat(commit_path, commit_id);
    char commit_index[FILENAME_SIZE]; // ./beargit/<commit_id>/.index
    strcpy(commit_index, commit_path);
    strcat(commit_index, dash);
    strcat(commit_index, index);
    
    f_index = fopen(commit_index, "r");
    
    while(fgets(line, sizeof(line), f_index)) {
        strtok(line, "\n");
        // copy the file into the current directory
        strcpy(file, commit_path);
        strcat(file, dash);
        strcat(file, line);
        fs_cp(file, line);
    }
    
    fclose(f_index);
    
    // copy over the index file
    fs_cp(commit_index, current_index);
    
    return 0;
}

//Helper function returns 1 if a directory in beargit is the commit id's and 0 if there isn't one
int is_it_a_commit_id(const char* commit_id) {
    char dirpath[FILENAME_SIZE];
    strcpy(dirpath, ".beargit/");
    strcat(dirpath, commit_id);
    
    return fs_check_dir_exists(dirpath);
}

int beargit_checkout(const char* arg, int new_branch) {
    // Get the current branch
    char current_branch[BRANCHNAME_SIZE];
    ///Error 1 fixed (copying into current_branch)
    read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);
    
    strtok(current_branch, "\n");
    
    
    // If not detached, update the current branch by storing the current HEAD into that branch's file...
    if (strlen(current_branch)) {
        //Creating dirpath to new file for branch
        char current_branch_file[BRANCHNAME_SIZE+50];
        sprintf(current_branch_file, ".beargit/.branch_%s", current_branch);
        
        //Copying prev (head of branch) into into file
        fs_cp(".beargit/.prev", current_branch_file);
    }
    
    // Check whether the argument is a commit ID. If yes, we just stay in detached mode
    // without actually having to change into any other branch.
    if (is_it_a_commit_id(arg)) {
        char commit_dir[FILENAME_SIZE] = ".beargit/";
        strcat(commit_dir, arg);
        
        if (!fs_check_dir_exists(commit_dir)) {
            fprintf(stderr, "ERROR:  Commit %s does not exist.\n", arg);
            return 1;
        }
        
        // Set the current branch to none (i.e., detached).
        write_string_to_file(".beargit/.current_branch", "");
        
        return checkout_commit(arg);
    }
    
    // Just a better name, since we now know the argument is a branch name.
    const char* branch_name = arg;
    
    // Read branches file (giving us the HEAD commit id for that branch).
    int branch_exists = (get_branch_number(branch_name) >= 0);
    
    // Check for errors.
    if (!(!branch_exists || !new_branch)) {             //Case: branch_exists-true, new_branch-true
        fprintf(stderr, "ERROR:  A branch named %s already exists.\n", branch_name);
        return 1;
        
    ///Error 2 found (false and false case)
    } else if (!branch_exists && !new_branch) {         //Case: branch_exists-false, new_branch-false
        fprintf(stderr, "ERROR:  No branch or commit %s exists.\n", branch_name);
        return 1;
    }
    
    // File for the branch we are changing into.
    ///Error 3 found (can't concatenate to a pointer)
    char branch_file[BRANCHNAME_SIZE] = ".beargit/.branch_";
    strcat(branch_file, branch_name);
    
    // Update the branch file if new branch is created (now it can't go wrong anymore)
    if (new_branch) {
        FILE* fbranches = fopen(".beargit/.branches", "a");
        fprintf(fbranches, "%s\n", branch_name);
        fclose(fbranches);
        fs_cp(".beargit/.prev", branch_file);
    }
    
    write_string_to_file(".beargit/.current_branch", branch_name);
    
    // Read the head commit ID of this branch.
    char branch_head_commit_id[COMMIT_ID_SIZE];
    read_string_from_file(branch_file, branch_head_commit_id, COMMIT_ID_SIZE);
    
    // Check out the actual commit.
    return checkout_commit(branch_head_commit_id);
}

/* beargit reset
 *
 * See "Step 7" in the project spec.
 *
 */

int beargit_reset(const char* commit_id, const char* filename) {
    if (!is_it_a_commit_id(commit_id)) {
        fprintf(stderr, "ERROR:  Commit %s does not exist.\n", commit_id);
        return 1;
    }
    
    // Check if the file is in the commit directory
    const char* beargit = ".beargit/";
    const char* dash = "/";
    
    char commit_dir[FILENAME_SIZE];
    strcpy(commit_dir, beargit);
    strcat(commit_dir, commit_id);
    char commit_index[FILENAME_SIZE];
    strcpy(commit_index, commit_dir);
    strcat(commit_index, "/.index");
    
    FILE* findex = fopen(commit_index, "r");
    char line[FILENAME_SIZE];
    char file[FILENAME_SIZE];
    int found = 0;
    
    while(fgets(line, sizeof(line), findex)) {
        strtok(line, "\n");
        if (0 == strcmp(line, filename)) {
            found = 1;
        }
    }
    
    fclose(findex);
    
    if (!found) {
        fprintf(stderr, "ERROR:  File %s does not exist in Commit.\n", filename);
        return 1;
    }
    
    // Copy the file to the current working directory
    char file_in_commit[FILENAME_SIZE];
    strcpy(file_in_commit, commit_dir);
    strcat(file_in_commit, dash);
    strcat(file_in_commit, filename);
    
    fs_cp(file_in_commit, filename);
    
    // Add the file if it wasn't already there
    findex = fopen(".beargit/.index", "r");
    found = 0;
    
    while(fgets(line, sizeof(line), findex)) {
        strtok(line, "\n");
        if (0 == strcmp(line, filename)) {
            found = 1;
        }
    }
    
    fclose(findex);
    
    if (!found) {
        return beargit_add(filename);
    }
    
    return 0;
}

/* beargit merge
 *
 * See "Step 8" in the project spec.
 *
 */

int beargit_merge(const char* arg) {
    // Get the commit_id or throw an error
    char commit_id[COMMIT_ID_SIZE];
    if (!is_it_a_commit_id(arg)) {
        if (get_branch_number(arg) == -1) {
            fprintf(stderr, "ERROR:  No branch or commit %s exists.\n", arg);
            return 1;
        }
        char branch_file[FILENAME_SIZE];
        snprintf(branch_file, FILENAME_SIZE, ".beargit/.branch_%s", arg);
        read_string_from_file(branch_file, commit_id, COMMIT_ID_SIZE);
    } else {
        snprintf(commit_id, COMMIT_ID_SIZE, "%s", arg);
    }
    
    // Iterate through each line of the commit_id index and determine how you
    // should copy the index file over
    // Check if the file is in the commit directory
    const char* beargit = ".beargit/";
    const char* dash = "/";
    
    char commit_dir[FILENAME_SIZE];
    strcpy(commit_dir, beargit);
    strcat(commit_dir, commit_id);
    char commit_index[FILENAME_SIZE];
    strcpy(commit_index, commit_dir);
    strcat(commit_index, "/.index");
    char commit_file[FILENAME_SIZE];
    
    // index file in commit
    fs_cp(".beargit/.index", ".beargit/.newindex");
    FILE* fnew_index = fopen(".beargit/.newindex", "ab");
    FILE* fcommit_index = fopen(commit_index, "r");
    FILE* fcurrent_index = fopen(".beargit/.index", "r");
    char line[FILENAME_SIZE];
    char currentLine[FILENAME_SIZE];
    int found = 0;
    
    while(fgets(line, sizeof(line), fcommit_index)) {
        strtok(line, "\n");
        
        while(fgets(currentLine, sizeof(currentLine), fcurrent_index)) {
            strtok(currentLine, "\n");
            if (0 == strcmp(line, currentLine)) {
                found = 1;
            }
        }
        
        strcpy(commit_file, commit_dir);
        strcat(commit_file, "/");
        strcat(commit_file, line);
        if (!found) {
            // add to current index
            fprintf(fnew_index, "%s\n", line);
            
            printf("%s added\n", line);
        } else {
            // create conflict file
            printf("%s conflicted copy created\n", line);
            strcat(line, ".");
            strcat(line, commit_id);
        }
        // copy file from commit to current directory
        fs_cp(commit_file, line);
        
        found = 0;
    }
    
    fclose(fcommit_index);
    fclose(fcurrent_index);
    fclose(fnew_index);
    
    fs_mv(".beargit/.newindex", ".beargit/.index");
    
    return 0;
}
