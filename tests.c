#include "events.c"
#include "graphics.c"
#include "audio.c"
#include "text.c"

#define DEFAULT_STRING_LENGTH 50

char funcName[DEFAULT_STRING_LENGTH];

static int test_selectInstruction_add() {
    char string[DEFAULT_STRING_LENGTH] = "add";
    int result;

    strcpy(funcName, __func__);
    result = selectInstruction(string);
    
    return result;
}

static int test_selectInstruction_remove() {
    char string[DEFAULT_STRING_LENGTH] = "remove";
    int result;

    strcpy(funcName, __func__);
    result = selectInstruction(string);
    
    return result;
}

static int test_selectInstruction_set() {
    char string[DEFAULT_STRING_LENGTH] = "set";
    int result;

    strcpy(funcName, __func__);
    result = selectInstruction(string);
    
    return result;
}

static int test_selectInstruction_stop() {
    char string[DEFAULT_STRING_LENGTH] = "stop";
    int result;

    strcpy(funcName, __func__);
    result = selectInstruction(string);
    
    return result;
}

static int test_selectInstruction_invalidCommand() {
    char string[DEFAULT_STRING_LENGTH] = "randomwords";
    int result;

    strcpy(funcName, __func__);
    result = selectInstruction(string);
    
    return result;
}

static int test_extractNumberFromString_thousand() {
    char string[DEFAULT_STRING_LENGTH] = "button1000";
    int result;

    strcpy(funcName, __func__);
    result = extractNumberFromString(string);

    return result;
}

static int test_extractNumberFromString_negativeOne() {
    char string[DEFAULT_STRING_LENGTH] = "button-1";
    int result;

    strcpy(funcName, __func__);
    result = extractNumberFromString(string);

    return result;
}

static int test_extractNumberFromString_jumble() {
    char string[DEFAULT_STRING_LENGTH] = "hksd6373jf833";
    int result;

    strcpy(funcName, __func__);
    result = extractNumberFromString(string);

    return result;
}

static char* test_makeLowercase_someCaps() {
    char* string = malloc(DEFAULT_STRING_LENGTH);
    strcpy(string, "DrUmS(fatkick)");

    strcpy(funcName, __func__);
    makeLowercase(string);

    return string;
}

static char* test_makeLowercase_allCaps() {
    char* string = malloc(DEFAULT_STRING_LENGTH);
    strcpy(string, "DRUMS(FATKICK)");

    strcpy(funcName, __func__);
    makeLowercase(string);

    return string;
}

static char* test_makeLowercase_noCaps() {
    char* string = malloc(DEFAULT_STRING_LENGTH);
    strcpy(string, "synth(trance)");

    strcpy(funcName, __func__);
    makeLowercase(string);

    return string;
}

static char* test_extractFilenameFromPath_invalidPath() {
    char* string = malloc(DEFAULT_STRING_LENGTH);
    strcpy(string, "samplesFatkick.wav");

    strcpy(funcName, __func__);

    return extractFilenameFromPath(string);
}

static char* test_extractFilenameFromPath_validPath() {
    char* string = malloc(DEFAULT_STRING_LENGTH);
    strcpy(string, "samples/Fatkick.wav");

    strcpy(funcName, __func__);

    return extractFilenameFromPath(string);
}

static char* test_extractFilenameFromPath_noExtension() {
    char* string = malloc(DEFAULT_STRING_LENGTH);
    strcpy(string, "samples/Fatkickwav");

    strcpy(funcName, __func__);

    return extractFilenameFromPath(string);
}

static gchar* test_createButtonIDForIndex_negativeInt() {
    int index = -1;
    char* result;

    strcpy(funcName, __func__);
    result = createButtonIDForIndex(index);

    return result;
}

static gchar* test_createButtonIDForIndex_positiveInt() {
    int index = 1;
    char* result;

    strcpy(funcName, __func__);
    result = createButtonIDForIndex(index);

    return result;
}

static gchar* test_createButtonIDForIndex_zero() {
    int index = 0;
    char* result;

    strcpy(funcName, __func__);
    result = createButtonIDForIndex(index);

    return result;
}

static void assert_int_equal(int expected, int obtained) {
    if (expected == obtained) {
        printf("TEST: %50s %7s\n", funcName, "PASSED");
    }
    else {
        printf("TEST: %50s %7s\n", funcName, "FAILED");
    }
}

static void assert_string_equal(char* expected, char* obtained)
{
    if (strcmp(expected, obtained) == 0) {
        printf("TEST: %50s %7s\n", funcName, "PASSED");
    }
    else {
        printf("TEST: %50s %7s\n", funcName, "FAILED");
    }
}

static void assert_null(void* value) {
    if (value == NULL) {
        printf("TEST: %50s %7s\n", funcName, "PASSED");
    }
    else {
        printf("TEST: %50s %7s\n", funcName, "FAILED");
    }

}


int main(void) {

    assert_int_equal(ADD, test_selectInstruction_add());
    assert_int_equal(REMOVE, test_selectInstruction_remove());
    assert_int_equal(SET, test_selectInstruction_set());
    assert_int_equal(STOP, test_selectInstruction_stop());
    assert_int_equal(-1, test_selectInstruction_invalidCommand());

    assert_int_equal(1000, test_extractNumberFromString_thousand());
    assert_int_equal(1, test_extractNumberFromString_negativeOne());
    assert_int_equal(6373, test_extractNumberFromString_jumble());

    assert_string_equal("drums(fatkick)", test_makeLowercase_someCaps());
    assert_string_equal("drums(fatkick)", test_makeLowercase_allCaps());
    assert_string_equal("synth(trance)", test_makeLowercase_noCaps());

    assert_null(test_extractFilenameFromPath_invalidPath());
    assert_string_equal("Fatkickwav", test_extractFilenameFromPath_noExtension());
    assert_string_equal("Fatkick", test_extractFilenameFromPath_validPath());

    assert_null(test_createButtonIDForIndex_negativeInt());
    assert_string_equal("button1", test_createButtonIDForIndex_positiveInt());
    assert_string_equal("button0", test_createButtonIDForIndex_zero());

    return 0;
}