#include <stddef.h>
#include <stdio.h>
#include <string.h>
typedef struct {
	size_t count; // view of remaining characters
	const char *view; // pointer to starting char of current view
		    
	size_t _size; // real size of string 
	const char * _start; // pointer to starting character of string

} StringView;

void svReset(StringView *sv) {
	sv->count = sv->_size;
	sv->view  = sv->_start;
}
/*
 * accepts a StringView and range(a, b) where a < b
 * returns the slice of the StringView
 * */
void svSlice(StringView *sv, size_t a, size_t b) {
	if( a <= sv->count && (a + b) <= sv->count ) {
		size_t upperBound = (b > sv->count) ? sv->count : b; 
		size_t chopped = a + upperBound;
		sv->count -= chopped;
		sv->view += a;
	};
}

StringView svInit(const char *str) {
	size_t len = strlen(str);
	return (StringView) {
		.count = len,
		.view = str,
		._size = len,
		._start = str
	};
}

void svPrint(StringView *sv) {
	printf("%.*s\n", (int)sv->count, sv->view);
}
int main() {

	const char *str = "hello-world";
	StringView sv = svInit(str);
	svSlice(&sv, 5, 5);
	svPrint(&sv);

	svSlice(&sv, 0, 1);
	svPrint(&sv);

	svSlice(&sv, 0, 1);
	svPrint(&sv);

	svReset(&sv);
	svPrint(&sv);

}
