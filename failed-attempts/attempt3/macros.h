#ifndef TURTLE_MACROS_H
#define TURTLE_MACROS_H

/* What the below macros do is generate a enum and strings based on a list of names
 * - Useful for debugging when you dont want to make an array of strings manually
 */

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,
#define GENERATE_ENUM_LIST(MACRO, NAME) \
   enum NAME                            \
   {                                    \
      MACRO(GENERATE_ENUM)              \
   };

#ifdef DEBUG
#define GENERATE_ENUM_STRING_NAMES(MACRO, NAME) \
   static const char *NAME##_Strings[] = {      \
       MACRO(GENERATE_STRING)};
#define print(FOO) std::cout << FOO << '\n'
#else
#define GENERATE_ENUM_STRING_NAMES(MACRO, NAME)
#define print(FOO)
#endif

#define GENERATE_ENUM_DATA(MACRO, NAME) \
   GENERATE_ENUM_LIST(MACRO, NAME)      \
   GENERATE_ENUM_STRING_NAMES(MACRO, NAME)

#define SUCCESS 0
#define FAILURE 1

#endif //TURTLE_MACROS_H
