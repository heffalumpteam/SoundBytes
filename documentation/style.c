/*Please can we debate a style sheet so that we can all be coding in the same style?*/

/*RULE 1: FOLLOW NEILS HOUSE STYLES*/

/*I would vote to move to C99 so that we can use // comments.
  However, I would avoid declerations of variables mid way
  through a function (allowed in c99). As they encourage longer
  functions. An exception to this would be for(int i = 0:......){}
*/

/*Functions should be short. No longer than a page*/

void filename_function(void);
/*
    filename_ dictates that this function is accessible to those
    that have included filename.h
    This function should be as short and closed as possible to
    maintain the scope of filename.c
*/

void filenameFunction(void);
/*
    camelCase shows function is only accessible within the scope of
    filename.c
    Does not need to include filename
*/

void function(void) {
  /*brace at end of line*/
}

if {

}
else if {

}
else{

}

for (i=0; i<x; i++) {

}

/* Pointer stars placed next to var names */
char *varname
