#include "Kure.h"
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char ** argv)
{
  int rows, cols;
  int i, j;
  KureContext * context = NULL;
  KureRel * rel = NULL;

  printf ("Please enter the dimension of your relation, e.g. '5 9' "
		  "(without quotes) for a relation with 5 rows and 9 columns:\n");

  if ( 2 != fscanf(stdin, "%d %d", &rows, &cols)) {
    printf ("First two numbers have to be the numbers of rows and cols"
	    "respectively.\n");
    return EXIT_FAILURE;
  }

  /* Create a new kure2 context. Contexts are background objects which 
   * are used to manage relations internally. You should use a single
   * context for all your relations since relations in different contexts
   * cannot, for instance, be compared or used in binary operations.
   * The context has a reference count of one and has to be references,
   * for instance, if passed to LUA routines. */
  context = kure_context_new ();
  kure_context_ref (context);

  /* Create a new relation with the given size. As you will very often
   * a bunch of functions is provided to do something with as few 
   * conversions as possible. Here, we create a relation of a given size
   * (_with_size) and we use signed integers (_si). In comparison, the
   * default type to store integers is GMP's mpz_t type. */
  rel = kure_rel_new_with_size_si (context, rows, cols);
  if (rel) {

	  printf ("Now, enter some pairs of numbers, for instance, '1 2' "
			  "(without quotes again) to set some bits in your "
			  "relation. The pair '1 2' sets the second bit in "
			  "the first row. You can stop with CTRL-D at any time."
			  "The relation is written to the screen afterwards:\n");

    /* Read some pairs of numbers. May separated by newlines. */
    while (!feof (stdin) && 2 == fscanf(stdin, "%d %d", &i, &j)) {

      /* Set the given bit. Again, we use signed integers here (_si). 
       * Most function in kure provide error checking. To this end, 
       * Kure_success (TRUE/FALSE) is returned. Either as the return value
       * or as an argument as in kure_get_bit_si. */
      if ( !kure_set_bit_si (rel, 1/*true*/, i,j)) {
	KureError * err = kure_context_get_error (context);
	printf ("Unable to set bit %d:%d (row,col) relation. Reason: %s.\n", 
		i,j,err?err->message:"Unknown");

	/* The error object is managed internally, so we don't have
	 * to care about memory management here. */
      }
    }

    /* NULL for psuccess means that we ignore the error. This is
     * dangerous if the number of entries can exceed INT_MAX */
    printf ("The relation has %d entries.\n",
	    kure_get_entries_si (rel, NULL));

    /* Print the relation to the screen. */
    for (i = 0 ; i < rows ; ++i) {
      for (j = 0 ; j < cols ; ++j) {
	if (kure_get_bit_si (rel, i,j, NULL))
	  putchar('X');
	else putchar('.');
      }
      putchar ('\n');
    }
  }
  else {
    KureError * err = kure_context_get_error (context);
    printf ("Unable to create relation. Reason: %s.\n", 
	    err?err->message:"Unknown");
  }

  /* Destroy the context. This also destroys all relations in this 
   * context. */
  kure_context_deref (context);

  return EXIT_SUCCESS;
}
