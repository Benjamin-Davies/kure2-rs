#include "Kure.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>
#include <errno.h>
#include <string.h> // strerror

/* Define DOT_FILE non-NULL to write DOT output to the given file. */
//#define DOT_FILE "hamilton.dot"

/*!
 * Output the given relation to the standard output. The routine uses GNU MP
 * mpz_t arbitrary size integers. The mpz_t interface is a little tedious to
 * use. However, it is fast and straightforward to use. In C++, GNU MP offers
 * a much simpler interface using mpz_class.
 *
 * GNU MP is available at http://www.gmplib.org .
 */
static void _dump_rel (KureRel * R)
{
	mpz_t i, j, rows ,cols;

	/* Initialize all necessary integers with 0. */
	mpz_init (i);
	mpz_init (j);
	mpz_init (rows);
	mpz_init (cols);

	/* Query the relation's number of rows and columns. */
	kure_rel_get_rows (R, rows);
	kure_rel_get_cols (R, cols);

	/* Traverse through all lines and columns. Print an 'X' for an entry and
	 * a '.' otherwise. */
	for ( ; mpz_cmp(i,rows) < 0 ; mpz_add_ui(i,i,1)) {
		for ( mpz_set_si(j,0) ; mpz_cmp(j,cols) < 0 ; mpz_add_ui(j,j,1)) {
			if (kure_get_bit (R, i, j, NULL)) putchar('X');
			else putchar('.');
		}
		putchar ('\n');
	}

	/* Cleanup. */
	mpz_clear (cols);
	mpz_clear (rows);
	mpz_clear (j);
	mpz_clear (i);
}


#ifdef DOT_FILE
/*!
 * Write the graph together with the resulting Hamilton path to the given
 * stream in DOT format. The edges of the path are highlighted. One can use
 * "dot -Tpng < file" in the command line to create a PNG image from that.
 */
static void _dump_dot_si (FILE * fp, KureRel * G, KureRel * H)
{
	int n, i, j;

	/* Check if the number of columns and the numbers of rows fits into signed
	 * integer type. If not, abort. Using signed integer instead of mpz_t
	 * simplifies things a lot and is completely sufficient here. In C++ things
	 * are much simpler. */
	assert (kure_rel_fits_si (G));

	/* Check if both relations have the same dimension. If not, abort. */
	assert (kure_rel_same_dim (G,H));

	/* Check if the given relations are graphs. Otherwise, abort. */
	assert (kure_is_hom (G, NULL));

	n = kure_rel_get_cols_si (G);

	fprintf (fp, "digraph {\n");
	for (i = 0 ; i < n ; i ++) {
		for (j = 0 ; j < n ; j ++) {
			/* If (i,j) is set in the relation, create an edge for it in the
			 * output. */
			if (kure_get_bit_si (G, i,j,NULL)) {
				fprintf (fp, "%d -> %d", i,j);

				/* Furthermore, if (i,j) are on the Hamilton path, highlight
				 * it. */
				if (kure_get_bit_si (H, i,j,NULL))
					fprintf (fp, " [color=\"#ff0000\"]");
				fputc ('\n', fp);
			}
		}
	}
	fprintf (fp, "}\n");
}
#endif

int main (int argc, char ** argv)
{
	/* We use a graph (homogenous relation), so the number and rows coincides
	 * anyway. */
	mpz_t n, ents;
	float prob = 0.5;
	KureContext * context = NULL;
	KureRel * G = NULL;
	struct timeval tv = {0};
	lua_State * L = NULL;

	/* The parameters are read from the command line. The second option is
	 * optional. */
	if (argc < 2) {
		printf ("usage: %s n [prob in ]0,1[\n", *argv);
		exit (EXIT_FAILURE);
	}

	mpz_init (n);
	gmp_sscanf (argv[1], "%Zd", n);

	if (argc > 2)
		sscanf (argv[2], "%f", &prob);
	if (prob <= 0 || prob >= 1) {
		printf ("Probability should be between 0 and 1 (both exclusive).\n");
		exit (EXIT_FAILURE);
	}

	/* Create a new kure2 context. Contexts are background objects which
	 * are used to manage relations internally. You should use a single
	 * context for all your relations since relations in different contexts
	 * cannot, for instance, be compared or used in binary operations.
	 * The context has a reference count of one and has to be references,
	 * for instance, if passed to LUA routines. */
	context = kure_context_new ();
	kure_context_ref (context);

	/* By default, the random() system call is used to create a random
	 * relation using \ref kure_random_simple. In order to not always get the
	 * same relation, we have to seed the pseudo random number generator. */
	gettimeofday (&tv, NULL);
	srandom (tv.tv_usec);

	/* Create a random relation given the probability. The preset random
	 * function is used here for simplicity. */
	G = kure_rel_new_with_size(context, n, n);
	if ( !G) {
		KureError * err = kure_context_get_error (context);
		printf ("Unable to create graph. Reason: %s", err?err->message:"Unknown");
		exit (EXIT_FAILURE);
	}
	kure_random_simple (G, prob);

	mpz_init (ents);
	kure_get_entries (G, ents);
	gmp_printf ("The random relation has %Zd entries.\n", ents);
	mpz_clear(ents);

	printf ("Random graph:\n");
	_dump_rel (G);

	/* Now create a new state for Lua and our domain-specific language
	 * (DSL). */
	L = kure_lua_new (context);
	if ( !L) {
		printf ("Unable to create Lua state.\n");
	}
	else {
		Kure_success success;
		KureError * err = NULL;

		/* Load some code from an external file. Use could also use
		 * \ref kure_lang_load to load string instead of files. Graph.prog
		 * depends on Aux.prog which provides a set of basic functions. */
		success = kure_lang_load_file (L, "Hamilton.prog", &err);
		if ( !success) {
			printf ("Unable to load file. Reason: %s\n", err->message);
			kure_error_destroy (err);
		}
		else {
			KureRel * H = NULL;

			/* In order to use a relation in the Lua state, you have to
			 * import it using \ref kure_lua_set_rel_copy which creates
			 * a copy of the given relation. However, note that copies of
			 * relations are very lightweight due to the underlying data
			 * structure of binary decision diagrams. */
			kure_lua_set_rel_copy(L, "G", G);

			/* Execute the given expression and save the result in a relation.
			 * In this case it would also be possible to use a call to Lua here.
			 * However, the DSL interface is somewhat more convenient. */
			H = kure_lang_exec(L, "Hamilton(G)", &err);
			if ( !H) {
				printf ("Unable to compute maximum matching. Reason: "
						"%s\n", err->message);
				kure_error_destroy(err);
			}
			else {
				if (kure_is_empty (H, NULL)) {
					printf ("No Hamilton path found.\n");
				}
				else {
					printf ("\nHamilton path:\n");
					_dump_rel (H);

#ifdef DOT_FILE
					{
						FILE * fp = fopen (DOT_FILE, "w+");
						if ( !fp) {
							printf ("Unable to open file "DOT_FILE". "
									"Reason: %s\n", strerror (errno));
						}
						else {
							_dump_dot_si (fp, G, H);
							fclose (fp);
						}
					}
#endif
				}

				kure_rel_destroy (H);
			}
		}

		kure_lua_destroy(L);
	}

	kure_rel_destroy (G);

  /* Destroy the context. This also destroys all relations in this 
   * context. */
  kure_context_deref (context);

  return EXIT_SUCCESS;
}

