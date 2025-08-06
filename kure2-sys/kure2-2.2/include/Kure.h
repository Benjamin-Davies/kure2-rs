/*
 * Kure.h
 *
 *  Copyright (C) 2010,2011,2012 Stefan Bolus, University of Kiel, Germany
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*!
\mainpage Kure2 Documentation

This documentation provides details on how to use the Kure2 library. See the
<a href="modules.html">modules page</a> on the left for an overview.

You should start by looking at \ref Contexts "contexts" (\ref KureContext). These are necessary to
create \ref Relations "relations" (\ref KureRel) which are the main objects of the library.
Relations can be manipulated in various ways. For instance, you can
\ref BasicFuncs "multiply" two relations using \ref kure_mult. More complex
operations like the \ref kure_symm_quotient "symmetric quotient" are available as well.

The Kure2 library comes with its own domain-specific programming language (DSL),
which allows you to develop relation algebraic programs more intuitively.
If you plan to use the DSL, you should take a look at the
\ref LuaAndLanguage "Lua language binding". The scripting language
<a href="http://www.lua.org/">Lua</a> has been used to
provide a powerful foundation for the DSL.

There are some examples that demonstrate specific use cases of the libraries. See
the <a href="examples.html">examples page</a> for details.

\example examples/basics.c
\example examples/dsl_to_lua.c
\example examples/dsl.c

 */

/*!
 * \file Kure.h
 */

#ifndef KURE_H_
#define KURE_H_

#ifdef __cplusplus
# include <cstdio>
# include <cstdlib>
# include <cstdarg>
#else
# include <stdio.h>
# include <stdlib.h>
# include <stdarg.h>
#endif

#include <gmp.h>
#include <lua.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "KureCudd.h"

/* \def KURE_VERBOSE
 *
 * If defined, Kure outputs some debugging output. The set is not fixed so this
 * feature is mainly for debugging purposes.
 */
//#define KURE_VERBOSE 1

#ifndef TRUE
# define TRUE 1
#endif

#ifndef FALSE
# define FALSE 0
#endif

/*! \defgroup Relations Relations
 * @{
 */

/*!
 * Opaque data type to represent a relation. Relations can be creates using,
 * for instance, \ref kure_rel_new.
 */
typedef struct _KureRel KureRel;
/*! @}  */

/*!
 * \defgroup DomainsTypes Domains and Types
 * @{
 */

/*!
 * Main class for domains. Subclasses are direct products and direct sums.
 */
typedef struct _KureDom KureDom;
/*! @} */

typedef struct _KureContext KureContext;

typedef char Kure_bool;

/*!
 * \defgroup ErrorHandling Error Handling
 *
 * @{
 */
typedef Kure_bool Kure_success;
/*! @} */

/*!
 * \defgroup LibVersion Library Version
 *
 * @{
 */

#define KURE_MAJOR_VERSION 2 /*!< The library's major version (always 2). */
#define KURE_MINOR_VERSION 2 /*!< The library's minor version. */
#define KURE_MICRO_VERSION 0 /*!< The library's micro version. */

/*!
 * Checks if the current library distribution has at least the given
 * version number. Returns TRUE or FALSE.
 */
#define KURE_CHECK_VERSION(major,minor,mirco)                                \
(((major) < KURE_MAJOR_VERSION)                                              \
 || ((major) == KURE_MAJOR_VERSION && (minor) < KURE_MINOR_VERSION)       \
 || ((major) == KURE_MAJOR_VERSION && (minor) == KURE_MINOR_VERSION       \
     && (micro) >= KURE_MICRO_VERSION))

/*!
 * Check whether a given Kure version number is not less than the
 * version number of the current Kure library.
 *
 * \return Returns 1 (true), if the current version is newer than the
 *         systems version number given, 0 (false) otherwise.
 */
int kure_check_version (int major, int minor, int micro);

/*! @} */


typedef enum _KureDirection
{
	KURE_DIR_DOWN_RIGHT = 1,
	KURE_DIR_DOWN_UP = 2,
	KURE_DIR_LEFT_RIGHT = 3,
	KURE_DIR_DOWN_LEFT = 4
} KureDirection;


/*!
 * Each occurrence of "$" is replaced by this definition. This is necessary
 * to allow "$" as a valid identifier inside the embedded programming
 * language. The dollar symbol is not a valid identifier in Lua.
 */
#define KURE_DOLLAR_SUBST "__dollar"


/*!
 * \addtogroup ErrorHandling
 *
 * @{
 */

typedef enum _KureErrorCode
{
	KURE_ERR_INV_ARG = 1,
	KURE_ERR_DIFF_CONTEXTS = 2, /*!< Relation are in different contexts. */
	KURE_ERR_NOT_SAME_DIM = 3, /*!< Relations of same dimension expected. (e.g. and) */
	KURE_ERR_NOT_COMPAT_DIM = 4, /*!< Relations are not compatible. (e.g. mult.) */
	KURE_ERR_OUT_OF_BOUNDS = 5, /*!< A given value (e.g. col,row) was out of bounds. */
	KURE_ERR_NUM_TOO_BIG = 6, /*!< A number was too big (>MAX_INT)*/
	KURE_ERR_SUCCESS
} KureErrorCode;


typedef struct _KureError {
	char * message;
	KureErrorCode code;
} KureError;
/*! @} */


/*!
 * Returns the number of variables necessary for the given number of rows/cols.
 * This function is rarely used outside library. If a relation is present, you
 * should use \ref kure_rel_get_vars_rows and \ref kure_rel_get_vars_cols
 * instead.
 */
int kure_num_vars (const mpz_t n);


typedef float (*KureRandomFunc) (void * /*user_data*/);

/*! \defgroup Contexts Contexts
 *
 * Contexts contain relations. Each relation is associated exactly to on
 * \ref KureContext object. Each time a new relation is created it belongs
 * to a context even if this binding is made implicitly.
 *
 * Contexts also store some other useful things like a default random function,
 * which is used by e.g. \ref kure_random_simple.
 *
 * Contexts can be created using \ref kure_context_new and destroyed using
 * \ref kure_context_destroy. It ends in undefined behavior if a context is
 * destroyed while there are relations associated to it. These relations become
 * immediately invalid and cannot be removed anymore.
 *
 * \ref KureContext objects are referenced counted. See \ref kure_context_ref
 * \ref kure_context_deref and \ref kure_context_get_refs.
 *
 * If not stated otherwise, relations in different contexts are incompatible.
 * That means, they must never be passed to a function together. E.g., calling
 * \ref kure_and with relations from different contexts is invalid and will
 * cause an error.
 *
 * @{
 */

/*!
 * Creates a new context with a newly created Cudd Manager. The context has a
 * reference count of zero. You should you the reference counting mechanism to
 * destroy the context.
 *
 * \see kure_context_new_with_manager
 * \see kure_context_ref
 * \see kure_context_deref
 * \see kure_context_destroy.
 *
 * \return The newly created context or NULL on error.
 */
KureContext * 	kure_context_new ();


/*!
 * Creates a new context with the given Cudd Manager. The context has a
 * reference count of zero. You should you the reference counting mechanism to
 * destroy the context.
 *
 * \warning You should not use a Cudd manager in more than one
 *          \ref KureContext.
 *
 * \see kure_context_new
 * \see kure_context_ref
 * \see kure_context_deref
 * \see kure_context_destroy.
 *
 * \return The newly created context or NULL on error.
 */
KureContext * 	kure_context_new_with_manager (DdManager * manager);

/*
 * Destroys the given context no matter of its reference count. This operation
 * is potentially dangerous and should not be used. Better use
 * \ref kure_context_deref.
 *
 * \note The relations which belong to the context are not destroyed but
 *       become invalid and cannot be destroyed anymore. So be sure that
 *       there are no such relations around.
 */
void 			kure_context_destroy (KureContext * self);

/*!
 * \return The associated BDD manager. Never returns NULL if the argument is
 *         valid.
 */
DdManager *		kure_context_get_manager (KureContext * self);

/*!
 * \return The current error object which may be NULL.
 */
KureError *		kure_context_get_error (KureContext * self);

/*!
 * \return The current random number generator. Never returns NULL if the
 *         argument is valid.
 */
KureRandomFunc 	kure_context_get_random_func (KureContext * self);

/*!
 * \return The current user-data for the random number generator. Never
 *         returns NULL if the argument is valid.
 */
void * 			kure_context_get_random_udata (KureContext * self);

/*!
 * Sets the current random number generator or resets it if random_func is
 * NULL. In the latter case, udata is ignored completely. Otherwise udata is
 * passed to the random function on each call.
 */
void 			kure_context_set_random_func (KureContext * self,
					KureRandomFunc random_func, void * udata);

/*!
 * Returns a random value with the current random function. This is just
 * a shortcut for:
 *
 *    void * udata;
 *    KureRandomFunc f = kure_context_get_random_func (context, &udata);
 *    return (f(udata));
 *
 * \return The returned value is in [0,1].
 */
float kure_context_random (KureContext * context);


/*!
 * In most situations, reference counting is irrelevant with a
 * \ref KureContext. Like usual, there are operations to increase and decrease
 * the counts and if it reaches 0, the object is destroyed. Alternatively, one
 * can use \ref kure_context_destroy to immediately destroy a context. In such
 * a situation, if the reference count is greater than one, a warning is
 * printed on the screen.
 *
 * \see kure_context_deref, kure_context_get_refs
 */
void kure_context_ref (KureContext * self);

/*!
 * Decreases the context's reference count and destroys it, if it was 1
 * before the call.
 *
 * \note The relations which belong to the context are not destroyed but
 *       become invalid and cannot be destroyed anymore. So be sure that
 *       there are no such relations around.
 *
 * \see kure_context_ref
 */
void kure_context_deref (KureContext * self);

/*!
 * Returns the reference count of the given \ref KureContext. Returns 0 if the
 * given argument is NULL.
 *
 * \see kure_context_ref, kure_context_deref
 *
 * \return The reference count.
 */
int kure_context_get_refs (KureContext * self);

/*!
 * @}
 */

/*!
 * \addtogroup ErrorHandling
 *
 * @{
 */

/*!
 * Creates a new \ref KureError object, which indicates success
 * (\ref KURE_ERR_SUCCESS). It has no associated message.
 */
KureError * 	kure_error_new ();

/*!
 * Destroys a given \ref KureError object which may be NULL.
 */
void 			kure_error_destroy (KureError * err);

/*!
 * Sets an error if perr is non-NULL. Does nothing otherwise. A \ref gmp_printf
 * compatible operation is used to create the message. The fmt argument may be
 * NULL. In this case the message would be set to "Unknown".
 */
void 			kure_set_error (KureError ** perr, KureErrorCode code, const char * fmt, ...);

/*!
 * Does the same as \ref kure_set_error.
 */
void 			kure_set_error_va (KureError ** perr, KureErrorCode code, const char * fmt, va_list ap);

/*! @} */

/*!
 * Convenience operation to set the associated \ref KureError object of the
 * given context. See \ref kure_set_error or \ref kure_set_error_va for
 * defails.
 */
void			kure_context_set_error (KureContext * context, KureErrorCode code, const char * fmt, ...);


/*!
 * \addtogroup Relations Relations and Operations
 *
 * @{
 */

/*!
 * Returns a new and empty relation with one row and one column in the given
 * \ref KureContext.
 *
 * \see kure_rel_with_size
 *
 * \return The newly created relation on success and NULL otherwise.
 */
KureRel * 		kure_rel_new (KureContext * context);

/*!
 * Returns a copy of the given relation in the same manager.
 *
 * \return The newly created copy on success and NULL otherwise.
 */
KureRel * 	    kure_rel_new_copy (const KureRel * R);

/*!
 * Create an empty relation with the given number of rows and columns in the
 * given \ref KureContext.
 *
 * \see kure_rel_new_with_size_si
 * \see kure_rel_new
 * \see kure_rel_new_with_rows
 * \see kure_rel_new_with_cols
 *
 * \return The newly created relation on success and NULL otherwise.
 */
KureRel * 		kure_rel_new_with_size (KureContext * context, const mpz_t rows, const mpz_t cols);

/*!
 * Returns an empty relation with the given number of rows and one column in
 * the given \ref KureContext.
 *
 * \see kure_rel_new_with_size
 * \see kure_rel_new_with_cols
 * \return The newly created relation on success and NULL otherwise.
 *  */
KureRel * 		kure_rel_new_with_rows (KureContext * context, const mpz_t rows);

/*!
 * Similar to \ref kure_rel_with_cols, but here the number of columns is given
 * and the rows are set to one.
 *
 * \see kure_rel_new_with_size
 * \see kure_rel_new_with_rows
 *
 * \return The newly created relation on success and NULL otherwise.
 */
KureRel * 		kure_rel_new_with_cols (KureContext * context, const mpz_t cols);

/*!
 * Similar to \ref kure_rel_new_with_size, but takes the number of rows and
 * columns as integers.
 *
 * \see kure_rel_new_with_size.
 *
 * \return The newly created relation on success and NULL otherwise.
 */
KureRel * 		kure_rel_new_with_size_si (KureContext * context, int rows, int cols);

/*!
 * Creates an empty relation of a given size. The size, as well as the
 * \ref KureContext, is passed implicitly using the given relation.
 *
 * \see kure_rel_new_with_size
 *
 * \return The newly created relation on success and NULL otherwise.
 */
KureRel * 		kure_rel_new_with_proto (const KureRel * proto);

/*!
 * Creates a relation from the given string data which must not be NULL. The
 * relation is created with the given size. Its contents is set w.r.t the
 * characters in s. The relation is filled line-wise, i.e. the first 'cols'
 * relevant characters in s determine the first row of the relation and so on.
 * Only characters one_ch and zero_ch are relevant. Consequently, all others
 * are ignored. This allows to use formating characters like spaces and
 * newlines. Whenever a one_ch character appears in s, the current bit is set
 * in the relation. All other bits remain unset.
 *
 * Reading stops if the string ends or rows x cols relevant characters were
 * read from it. I.e. if s ends before that number is read the remaining bits
 * remain unset.
 *
 * \return Returns NULL on error (e.g. something is NULL, rows<0, etc.)
 */
KureRel * 		kure_rel_new_from_string (KureContext * context, const char * s,
					int rows, int cols,	char one_ch, char zero_ch);

Kure_success 	kure_rel_assign_from_string (KureRel * self, const char * s,
					int rows, int cols,	const char one_ch, const char zero_ch);



/*!
 * Creates a node relation from the given BDD and the given size. If the
 * function succeeds, the node is references by the relation.
 *
 * \note If the given node has a reference count of 0 and the function fails,
 *       that is, it returns NULL, the node is still valid and still has a
 *       reference count of 0.
 *
 * \see kure_rel_new_with_size
 */
KureRel *		kure_rel_new_from_bdd (KureContext * context, DdNode * node,
					const mpz_t rows, const mpz_t cols);
KureRel * 		kure_rel_new_from_bdd_si (KureContext * context, DdNode * node, int rows, int cols);
void 			kure_rel_destroy (KureRel * self);

Kure_bool 		kure_rel_fits_si (const KureRel * self);
Kure_bool		kure_rel_prod_fits_si (const KureRel * self);
Kure_bool 		kure_rel_rows_fits_si (const KureRel * self);
Kure_bool 		kure_rel_cols_fits_si (const KureRel * self);
void 			kure_rel_get_rows (const KureRel * self, mpz_t rows);
void 			kure_rel_get_cols (const KureRel * self, mpz_t cols);
int 			kure_rel_get_rows_si (const KureRel * self);
int 			kure_rel_get_cols_si (const KureRel * self);
Kure_bool 		kure_rel_same_dim(const KureRel * a, const KureRel * b);

int 			kure_rel_get_vars_cols (const KureRel * self);
int				kure_rel_get_vars_rows (const KureRel* self);

/*!
 * Sets the size of the given relation.
 *
 * \warning This is no scaling operation. The contents of the relation is
 *          undefined after this operation. It's intention is to be used
 *          internally.
 */
Kure_success 	kure_rel_set_size (KureRel * self, const mpz_t rows, const mpz_t cols);
Kure_success 	kure_rel_set_size_si (KureRel * self, int rows, int cols);

/*!
 * Returns the relations \ref KureContext object. Never returns NULL.
 */
KureContext * 	kure_rel_get_context (KureRel * self);


/*!
 * Returns the BDD node currently in use. The node has to be referenced in
 * order to be used externally. Use \ref Cudd_Ref to reference it and use
 * Cudd_RecursiveDeref to dereference it. The \ref DdManager is available
 * from the context (\ref kure_rel_get_context and
 * \ref kure_context_get_manager).
 *
 *  In order to apply changed to a relation you have to use \kure_set_bdd.
 *
 *  \see kure_set_bdd
 */
DdNode * 		kure_rel_get_bdd (const KureRel * self);


/*!
 * Sets the current BDD node of the given relation. Recursively dereferences
 * to old BDD node and references the given node. The caller should dereference
 * the node after this call if it is no longer needed. Use \ref Cudd_Deref if
 * you now what you do and \ref Cudd_RecursiveDeref or \ref Cudd_IterDerefBdd
 * otherwise.
 */
void			kure_rel_set_bdd (KureRel * self, DdNode * node);


/*!
 * Converts the given relation into a string of length rows*cols+rows.
 * Each row is terminated by a newline ('\n'). If a bit is set one_ch
 * is used and zero_ch otherwise.
 *
 * \param rel The relation to convert.
 * \param one_ch Character used if a bit is set.
 * \param zero_ch Character used if a bit is NOT set. Can be '\0'.
 */
char * 			kure_rel_to_string (KureRel * rel, char one_ch, char zero_ch);

/*!
 *
 */
Kure_success kure_rel_read_from_dddmp_stream (KureRel * self, FILE * fp,
		const mpz_t rows, const mpz_t cols);

Kure_success kure_rel_read_from_dddmp_file (KureRel * self, const char * file,
		const mpz_t rows, const mpz_t cols);

/*!
 *
 */
Kure_success kure_rel_write_to_dddmp_stream (const KureRel * rel, FILE * fp);

Kure_success kure_rel_write_to_dddmp_file (const KureRel * rel, const char * file);

/*!
 * \internal
 *
 * Operations have the following idea (like GMP):
 * 	 rop <- OP(arg1,arg2,...)
 *
 * which leads to a prototype
 *    OP(rop, arg1, arg2, ...)
 *
 * Unfortunately, this is conversely to what is used in the internal interface
 * and won't be changed for historic reasons.
 *
 * Another point is the order of row(s)/cols(s) argument. The order in use is
 * the mathematical order rows before cols (and row before col). This may also
 * differ in the internal interface.
 *
 * Each operation which uses at least one argument can have equal arguments
 * for each parameter. Viz. each argument could be the same relation to allow
 * inplace operations. E.g. kure_rel_transp(r,r) transposes r and stores the
 * result in r again.
 *
 * Each operation returns a boolean value to indicate success (TRUE) or
 * failure (FALSE). Additionally, if a failure occurs for some functions,
 * the error message can be requested from the Kure_context.
 *
 * In general, all relations given as arguments have to belong to the same
 * context. The operation will fail otherwise. The same holds for NULL
 * argument or invalid sized (e.g. <=0)
 *
 * Some operations have multiple names but fulfill the very same task
 * (e.g. kure_O and kure_null, or kure_L and kure_all). These are just for
 * different naming preferences.
 *
 * Some operations have variants for GMP numbers and signed integers. The
 * latter are for sake of convenience and have a '_si' suffix (as GMP's ops.
 * have).
 *
 * Operations which regularly have a return value which cannot indicate an error
 * have an additional last
 * argument to poll the success of the operation. This value can be omitted
 * by passing NULL.
 *
 * Operations which work only on vectors have a 'vec_' prefix.
 */

///////////////////

/*!
 * \defgroup RelConsts Relational Constants
 * @{
 */

/*!
 * Sets the given relation to O, that is, \f$R:=\emptyset\f$.
 *
 * \param R The relation.
 * \returns TRUE on success, FALSE on error.
 */
Kure_success 	kure_O (KureRel * R);

/*!
 * Same as \ref kure_O.
 */
Kure_success	kure_null(KureRel * R);

/*!
 * Sets the given relation to L, that is, \f$\forall x,y: R_{x,y}\f$.
 *
 * \param R The relation.
 * \returns TRUE on success, FALSE on error.
 */
Kure_success 	kure_L (KureRel * R);

/*!
 * Same as \ref kure_L.
 */
Kure_success 	kure_all (KureRel * R);

/*!
 * Sets the given relation to I, that is, \f$R_{x,y}\iff x=y.\f$
 */
Kure_success 	kure_I (KureRel * R);

/*!
 * Same as \ref kure_I.
 */
Kure_success 	kure_identity (KureRel * R);

/*!
 * @}
 */

/*!
 * \defgroup BasicFuncs Basic Functions
 *
 * Functions covering basic functions which operate on one or more relations. For
 * each functions, if
 * not stated otherwise, arguments may overlap. That means that if more than
 * one relation is expected multiple occurrences of one relation is permitted.
 *
 * For instance, if R and S are relations, the following is valid:
 * \code
 * kure_transpose(S,S); // S = S^
 * kure_and(R,R,S); // R = R & S
 * \endcode
 *
 * @{
 */

/*!
 * Computes \f$rop_{x,y}\iff arg1_{x,y}\land arg2_{x,y}\f$.
 *
 * \see kure_or
 *
 * \param rop Relation to store the result in. Has to be a valid relation.
 * \param arg1
 * \param arg2
 * \return True on success and FALSE on error.
 */
Kure_success 	kure_and (KureRel * rop, const KureRel * arg1, const KureRel * arg2);

/*!
 * Computes \f$rop_{x,y}\iff arg1_{x,y}\lor arg2_{x,y}\f$.
 *
 * \see kure_and
 *
 * \param rop Relation to store the result in. Has to be a valid relation.
 * \param arg1
 * \param arg2
 * \return True on success and FALSE on error.
 */
Kure_success 	kure_or (KureRel * rop, const KureRel * arg1, const KureRel * arg2);

/*!
 * Computes the transpose of a given relation. Afterwards,
 * \f$rop_{x,y}\iff arg_{y,x}\f$.
 *
 * \param rop Relation to store the result in. Has to be a valid relation.
 * \param arg Argument relation.
 * \return Returns true on success and FALSE on error.
 */
Kure_success 	kure_transpose (KureRel * rop, const KureRel * arg);

/*!
 * Computes the complement of a given relation. Afterwards,
 * \f$rop_{x,y}\iff \neg arg_{x,y}\f$.
 *
 * \param rop Relation to store the result in. Has to be a valid relation.
 * \param arg Argument relation.
 * \return Returns true on success and FALSE on error.
 */
Kure_success 	kure_complement (KureRel * rop, const KureRel * arg);

/*!
 * Multiplies two relations. The relations have to be compatible in dimension.
 * That it, cols(arg1)=rows(arg2). The result has a size rows(arg1) x cols(arg2).
 *
 * \see kure_mult_transp_norm
 * \see kure_mult_norm_transp
 * \see kure_mult_transp_transp
 *
 * \param rop Relation to store the result in. Has to be a valid relation.
 * \param arg1 First argument.
 * \param arg2 Second argument.
 * \return True on success and FALSe on error.
 */
Kure_success	kure_mult (KureRel * rop, const KureRel * arg1, const KureRel * arg2);

/*!
 * Multiplies two relations and transposes the first of them. The relations
 * have to be compatible in dimension. That it, rows(arg1)=rows(arg2). The
 * result has a size cols(arg1) x cols(arg2).
 *
 * In RelView notation:
 * \code
 *    rop := arg1^ * arg2
 * \endcode
 *
 * \see kure_mult
 * \see kure_mult_norm_transp
 * \see kure_mult_transp_transp
 *
 * \param rop Relation to store the result in. Has to be a valid relation.
 * \param arg1 First argument.
 * \param arg2 Second argument.
 * \return True on success and FALSe on error.
 */
Kure_success	kure_mult_transp_norm (KureRel * rop, const KureRel * arg1, const KureRel * arg2);

/*!
 * Multiplies two relations and transposes the second of them. The relations
 * have to be compatible in dimension. That it, cols(arg1)=cols(arg2). The
 * result has a size rows(arg1) x rows(arg2).
 *
 * In RelView notation:
 * \code
 *    rop := arg1* arg2^
 * \endcode
 *
 * \see kure_mult
 * \see kure_mult_transp_norm
 * \see kure_mult_transp_transp
 *
 * \param rop Relation to store the result in. Has to be a valid relation.
 * \param arg1 First argument.
 * \param arg2 Second argument.
 * \return True on success and FALSe on error.
 */
Kure_success	kure_mult_norm_transp (KureRel * rop, const KureRel * arg1, const KureRel * arg2);

/*!
 * Multiplies two relations and transposes both of them. The relations
 * have to be compatible in dimension. That it, rows(arg1)=cols(arg2). The
 * result has a size cols(arg1) x rows(arg2).
 *
 * In RelView notation:
 * \code
 *    rop := arg1^* arg2^
 * \endcode
 *
 * \see kure_mult
 * \see kure_mult_transp_norm
 * \see kure_mult_norm_transp
 *
 * \param rop Relation to store the result in. Has to be a valid relation.
 * \param arg1 First argument.
 * \param arg2 Second argument.
 * \return True on success and FALSe on error.
 */
Kure_success	kure_mult_transp_transp (KureRel * rop, const KureRel * arg1, const KureRel * arg2);

/*!
 * @}
 */


/*!
 * \defgroup BitAccess Bit Level Access
 *
 * Functions in this module can be used to add pairs (x,y) to a relation and to
 * test if a given pair is in a relation. From another point of view, these
 * functions provide the ability to set, unset and get individual entries (bits)
 * of a relation represented by a Boolean matrix.
 * @{
 */

/*!
 * Sets a given bit to yes (TRUE) or no (FALSE).
 *
 * \see kure_set_bit_si
 *
 * \param R The relation to manipulate.
 * \param yesno TRUE to set, FALSE to unset the bit.
 * \param row The row of the bit. (0-indexed)
 * \param cols The column of the bit. (0-indexed)
 * \return TRUE on success and FALSE on error.
 */
Kure_success 	kure_set_bit (KureRel * R, Kure_bool yesno, mpz_t row, mpz_t col);

/*!
 * Same as \ref kure_set_bit, except that here the row and column are integers
 * of limited size.
 */
Kure_success 	kure_set_bit_si (KureRel * R, Kure_bool yesno, int row, int col);

/*!
 * Returns the status of a given bit.
 *
 * \see kure_get_bit_fast
 * \see kure_get_bit_si
 * \see kure_get_bit_fast_si
 *
 * \param R The relation.
 * \param row The row of the bit. (0-indexed)
 * \param col The column of the bit. (0-indexed)
 * \param [out] psuccess Used to return whether the call was successful.
 * 					     May be NULL.
 */
Kure_bool 		kure_get_bit (KureRel * R, mpz_t row, mpz_t col, Kure_success * psuccess);

/*!
 * Same as \ref kure_get_bit, except that:
 *   - No error checking is performed and
 *   - the number of variables has to be passed explicitly.
 *
 * One can use \ref kure_num_vars to obtain the number of variables for the
 * rows and columns, respectively. In contrast to \ref kure_get_bit, this
 * function should be much faster when many bits are needed.
 */
Kure_bool 		kure_get_bit_fast (KureRel * R,mpz_t row, mpz_t col, int vars_rows, int vars_cols);

/*!
 * Same as \ref kure_get_bit, except that here the row and column are integers
 * of limited size.
 */
Kure_bool 		kure_get_bit_si (KureRel * R, int row, int col, Kure_success * psuccess);

/*!
 * The faster version of \ref kure_get_bit_si. See \ref kure_get_bit_fast for
 * details.
 */
Kure_bool 		kure_get_bit_fast_si (KureRel * R, int row, int col, int vars_rows, int vars_cols);

/*!
 * @}
 */

/*!
 * \defgroup LineAccess Rows and Columns
 *
 * Rows play an important role in the context of relational vectors and
 * the representation of sets.
 * @{
 */

/*!
 * Sets or unsets the given row.
 *
 * \see kure_set_row_si
 *
 * \param R The relation.
 * \param yesno TRUE to set and FALSE to unset the row.
 * \param row The row to set or unset.
 * \return Returns TRUE on success and FALSE on error.
 */
Kure_success	kure_set_row (KureRel * R, Kure_bool yesno, mpz_t row);

/*!
 * Same as \ref kure_set_row, except that here the row is and integer of
 * limited size.
 */
Kure_success	kure_set_row_si (KureRel * R, Kure_bool yesno, int row);

/*!
 * @}
 */

/*!
 * \defgroup Cardinality
 *
 * If you need the number of elements in a relation and/or vector, see also \ref Entries.
 *
 * @{
 */

/*!
 * Creates a cardinality comparison relation of a given size. Let n be the
 * number of rows of arg. Then it creates a Relation R with \f$2^n\f$ rows and
 * \f$2^n\f$ columns such that for each pair of vectors v,w with one column
 * we have \f$(v,w)\in R \iff |v| < |w|\f$.
 */
Kure_success 	kure_less_card (KureRel * rop, const KureRel * arg);

/*!
 * Filters a sets w.r.t. to a given cardinality contraint. Argument pot has to
 * be a vector of size \f$2^k\f$. Then \f$rop_{w}\f$ iff \f$vec_{w}\land |w|<rows(vec)\f$.
 * E.g. if vec has just 1 row, and pot is L, then only \f$\emptyset\f$ is in rop. If
 * vec is equal to k+1 all elements pass the filter.
 *
 * \param rop The filtered set. Has to be initialized.
 * \param pot The vector to be filtered. Its size has to be a power of 2. Can have
 *            any number of columns as long it is a vector.
 * \param vec The vector which specifies the size. If pot has a size of 2^k,
 *            must not have more than k+1 rows. The number of columns is ignored.
 * \param TRUE on success and FALSe on error.
 */
Kure_success 	kure_subsetvec_rel (KureRel * rop, const KureRel * pot, const KureRel * vec);


/*!
 * Tests if the cardinalities of the given relations are equal.
 *
 * \see kure_is_cardlt
 * \see kure_is_cardleq
 *
 * \param R The first relation.
 * \param S The second relation.
 * \param [out] psuccess Returns TRUE on success and FALSE on error. May be NULL.
 * \return Returns TRUE iff \f$|R| = |S|\f$.
 */
Kure_bool		kure_is_cardeq (const KureRel * R, const KureRel * S, Kure_success *psuccess);

/*!
 * Tests if the cardinality of a relation is less than the cardinality of
 * another relations (less than).
 *
 * \see kure_is_cardeq
 * \see kure_is_cardlt
 *
 * \param R The first relation.
 * \param S The second relation.
 * \param [out] psuccess Returns TRUE on success and FALSE on error. May be NULL.
 * \return Returns TRUE iff \f$|R| < |S|\f$.
 */
Kure_bool		kure_is_cardlt (const KureRel * R, const KureRel * S, Kure_success *psuccess);

/*!
 * Tests if the cardinalities of the given relations are equal (less or equal)
 *
 * \see kure_is_cardeq
 * \see kure_is_cardlt
 *
 * \param R The first relation.
 * \param S The second relation.
 * \param [out] psuccess Returns TRUE on success and FALSE on error. May be NULL.
 * \return Returns TRUE iff \f$|R| <= |S|\f$.
 */
Kure_bool		kure_is_cardleq (const KureRel * R, const KureRel * S, Kure_success *psuccess);

/*!
 * @}
 */


/*!
 * \defgroup Entries Counting Entries
 *
 *
 *
 * @{
 */

/*!
 * Counts the numbers of entries in the given relation.
 *
 * \param R The initialized relation.
 * \param [out] entries The number of entries. Has to be initialized.
 * \return Returns TRUE on success and FALSE on error.
 */
Kure_success	kure_get_entries (const KureRel * R, mpz_t entries);

/*!
 * Same as \ref kure_get_entries, except that the number of entries is returned
 * as a integer of limited size. If the number of entries is doesn't fit into
 * an integer, psuccess is set to FALSE. A(ny) non-negative number is returned
 * anyway.
 *
 * \param [out] psuccess Returns TRUE on success and FALSE on error. May be NULL.
 * \return The number of entries or a random number if the number of entries is
 *         too big.
 */
int				kure_get_entries_si (const KureRel*, Kure_success * psuccess);

/*!
 * Returns the number of entries in a vector. This is different from the number
 * of entries in a relation if the vector has more than one column. For a
 * vector an entry is a filled row.
 */
Kure_success	kure_vec_get_entries (const KureRel*, mpz_t entries); // rvRelationCountColumns

/*!
 * Same as \ref kure_vec_get_entries, except that here an integer is returned.
 * See also \ref kure_get_entries_si.
 */
int				kure_vec_get_entries_si (const KureRel*, Kure_success * psuccess);

/*!
 * @}
 */

/*!
 * \defgroup Hulls Hulls
 * @{
 */

/*!
 * Computes the transitive hull (closure) of the given argument.
 *
 * \param rop The relation to store the result in. Has to be initialized.
 * \param arg The argument.
 * \return Returns TRUE on success and FALSE on error.
 */
Kure_success	kure_trans_hull (KureRel * rop, const KureRel * arg);

/*!
 * Computes the symmetric hull (closure) of the given argument. That is:
 * \code
 * rop := arg | arg^
 * \endcode
 *
 * \param rop The relation to store the result in. Has to be initialized.
 * \param arg The argument.
 * \return Returns TRUE on success and FALSE on error.
 */
Kure_success	kure_symm_hull (KureRel * rop, const KureRel * arg);

/*!
 * Computes the reflexive hull (closure) of the given argument. That is:
 * \code
 * rop := arg | I
 * \endcode
 * \param rop The relation to store the result in. Has to be initialized.
 * \param arg The argument.
 * \return Returns TRUE on success and FALSE on error.
 */
Kure_success	kure_refl_hull (KureRel * rop, const KureRel * arg);

/*!
 * @}
 */


/*!
 * \defgroup SpecialSets Special Sets and Relations
 *
 *
 *
 * @{
 */

/*!
 * Creates the membership relation for a given size.
 *
 * \param rop The relation to store the result in. Has to be initialized.
 *            Dimension is n x 2^n.
 * \param n Number of elements.
 * \return Returns TRUE on success and FALSE on error.
 */
Kure_success	kure_membership (KureRel * , int n);

/*!
 * Partial functions. An exact specification is given by the following code:
 * \code
  	int i,j,k,col,factor = 1;

 	set rows to size1*size2;
 	set cols to (size2+1)^size1;
	clear relation;

 	for (i = 0 ; i < size1 ; ++i) {
		for (j = 0 ; j < size2 ; ++j) {
			for (col = (j+1)*factor ; col < rop->cols ; col += factor * (size2+1))
				for (k = 0 ; k < factor ; ++k)
					set bit at row j+(i*size2) and col k+col;
		}
		factor *= size2+1;
	}
 \endcode
 */
Kure_success	kure_partial_funcs (KureRel*, int size1, mpz_t size2);

/*!
 * Same as \ref kure_partial_funcs but takes an integer for the second size
 * argument.
 *
 * \see kure_partial_funcs
 * \see kure_total_funcs_si
 */
Kure_success	kure_partial_funcs_si (KureRel*, int size1, int size2);

/*!
 * Total functions. An exact specification is given by the following code:
 * \code
	int i,j,k,col,factor = 1;

 	set rows to size1*size2;
 	set cols to size2^size1;
 	clear relation;

	for (i = 0 ; i < size1 ; ++i) {
		for (j = 0 ; j < size2 ; ++j) {
			for (col = j*factor ; col < rop->cols ; col += factor * size2)
				for (k = 0 ; k < factor ; ++k)
					set bit at row j+(i*size2) and col k+col;
		}
		factor *= size2;
	}
 \endcode
 */
Kure_success	kure_total_funcs (KureRel*, int size1, mpz_t size2);

/*!
 * Same as \ref kure_total_funcs but takes an integer for the second size
 * argument.
 *
 * \see kure_total_funcs
 * \see kure_partial_funcs_si
 */
Kure_success	kure_total_funcs_si (KureRel*, int size1, int size2);

/*!
 * \note NOT DOCUMENTED.
 */
Kure_success	kure_successors (KureRel * R, mpz_t size);

/*!
 * \note NOT DOCUMENTED.
 */
Kure_success	kure_successors_si (KureRel * R, int size);

/*!
 * Computes a vector containing the domain of R, that is, a vector v such that
 * rows(v) = rows(R) and \f$rows(v)_x \iff \exists y: R_{x,y}\f$.
 *
 * \param rop The relation to store the result in. Has to be initialized. Will
 *            have the same number of rows than the input. Will have exactly
 *            one column.
 * \return Returns TRUE on success and FALSE on error.
 */
Kure_success	kure_domain (KureRel * v, const KureRel * R);

/*!
 * @}
 */

/*!
 * \defgroup Random Random Relations
 *
 * There are currently three different types of random relations which can be
 * created: Arbitrary relations, relations without cycles and permutations.
 * Permutations are just bijective functions.
 *
 * For each random function there is a corresponding simplified version which
 * doesn't expect a random function. These functions use the random function
 * of the \ref KureContext argument which is implicitly passed by the relation.
 *
 * The version with a _full suffix first resizes the relation and then calls
 * the corresponding non-full version.
 *
 * @{
 */

/*!
 * Creates an arbitrary random relation using the random function of the
 * relation's \ref KureContext. The result has the same dimension as the input.
 *
 * \see kure_random
 * \see kure_random_full
 * \see kure_random_full_si
 *
 * \param R The relation to set. Has to be initialized.
 * \param prob Each entry is in the relation with probability 1/prob.
 * \return Returns TRUE on success and FALSE on error.
 */
Kure_success	kure_random_simple (KureRel * R, float prob);

/*!
 * Creates an arbitrary random relation using the given \ref KureRandomFunc.
 * The result has the same dimension as the input.
 *
 * \see kure_random_simple
 * \see kure_random_full
 * \see kure_random_full_si
 *
 * \param R The relation to set. Has to be initialized.
 * \param prob Each entry is in the relation with probability 1/prob.
 * \param random_func The random function used to set each bit.
 * \param user_data Is passed to the random function.
 * \return Returns TRUE on success and FALSE on error.
 */
Kure_success	kure_random (KureRel * R, float prob, KureRandomFunc random_func, void * user_data);

/*!
 * Creates an arbitrary random relation using the given \ref KureRandomFunc and
 * the given size. The result has the specified dimension. See \ref kure_random
 * for details.
 */
Kure_success	kure_random_full (KureRel * R,mpz_t rows, mpz_t cols,float prob,KureRandomFunc random_func, void *user_data);

/*!
 * Creates an arbitrary random relation using the given \ref KureRandomFunc and
 * the given size. The result has the specified dimension. Same as
 * \ref kure_random_full for integer numbers.
 */
Kure_success	kure_random_full_si (KureRel * R,int rows,int cols,float prob,KureRandomFunc random_func, void *user_data);

/*!
 * Creates a random relation without cycles using the random function of
 * the relation's \ref KureContext. The result has the same dimension as the
 * input.
 *
 * \see kure_random_no_cycles
 * \see kure_random_no_cycles_full
 * \see kure_random_no_cycles_full_si
 *
 * \param R The relation to set. Has to be initialized.
 * \param prob Each entry is in the relation with probability 1/prob.
 * \return Returns TRUE on success and FALSE on error.
 */
Kure_success	kure_random_no_cycles_simple (KureRel * R,float prob);

/*!
 * Creates a random relation without cycles using the given
 * \ref KureRandomFunc. The result has the same dimension as the input.
 *
 * \see kure_random_no_cycles_simple
 * \see kure_random_no_cycles_full
 * \see kure_random_no_cycles_full_si
 *
 * \param R The relation to set. Has to be initialized.
 * \param prob Each entry is in the relation with probability 1/prob.
 * \param random_func The random function used to set each bit.
 * \param user_data Is passed to the random function.
 * \return Returns TRUE on success and FALSE on error.
 */
Kure_success	kure_random_no_cycles (KureRel * R,float prob,KureRandomFunc random_func, void *user_data);

/*!
 * Creates a random relation without cycles using the given \ref KureRandomFunc and
 * the given size. The result has the specified dimension. See
 * \ref kure_random_no_cycles for details.
 */
Kure_success	kure_random_no_cycles_full (KureRel * R,mpz_t size,float prob,KureRandomFunc random_func, void *user_data);

/*!
 * Create a random relation without cycles using the given \ref KureRandomFunc
 * and the given size. The result has the specified dimension. Same as
 * \ref kure_random_no_cycles_full for integer numbers.
 */
Kure_success	kure_random_no_cycles_full_si (KureRel * R,int size,float prob,KureRandomFunc random_func, void *user_data);

/*!
 * Creates a random permutation using the random function of the
 * relation's \ref KureContext. The result has the same dimension as the
 * input. The relation has to be homogeneous.
 *
 * \see kure_random_perm
 * \see kure_random_perm_full
 * \see kure_random_perm_full_si
 *
 * \param R The relation to set. Has to be initialized. Has to be homogeneous.
 * \return Returns TRUE on success and FALSE on error.
 */
Kure_success	kure_random_perm_simple (KureRel * R);

/*!
 * Creates a random permutation using the given random function. The result
 * has the same dimension as the input. The relation has to be homogeneous.
 *
 * \see kure_random_perm_simple
 * \see kure_random_perm_full
 * \see kure_random_perm_full_si
 *
 * \param R The relation to set. Has to be initialized. Has to be homogeneous.
 * \return Returns TRUE on success and FALSE on error.
 */
Kure_success kure_random_perm (KureRel * self, KureRandomFunc random_func, void * user_data);

/*!
 * Creates a random relation without cycles using the given \ref KureRandomFunc and
 * the given size. The result is homogeneous and has the specified dimension.
 */
Kure_success	kure_random_perm_full (KureRel * R,mpz_t size, KureRandomFunc random_func, void *user_data);

/*!
 * Creates a random relation without cycles using the given \ref KureRandomFunc and
 * the given size. The result is homogeneous and has the specified dimension.
 * Same as \ref kure_random_perm_full for integers numbers.
 */
Kure_success	kure_random_perm_full_si (KureRel * R,int size, KureRandomFunc random_func, void *user_data);

/*!
 * @}
 */

// Residue and Symmetric Quotient
/*!
 * \defgroup ResQuot Residue and Symmetric Quotient
 *
 *
 *
 * @{
 */

/*!
 * Computes the left residue of R and S, that is:
 * \code
 * rop := S/R = -(-S*R^)
 * \endcode
 *
 * As you can see from the code above, the relations R and S need to have
 * compatible dimensions.
 *
 * \param rop The relation to set. Has to be initialized.
 * \param R The first argument.
 * \param S The second argument.
 * \return Returns TRUE on success and FALSE on error.
 */
Kure_success	kure_left_residue (KureRel * rop, const KureRel * S, const KureRel * R);

/*!
 * Computes the right residue of R and S, that is:
 * \code
 * rop := R\S = -(R^-S)
 * \endcode
 *
 * As you can see from the code above, the relations R and S need to have
 * compatible dimensions.
 *
 * \param rop The relation to set. Has to be initialized.
 * \param R The first argument.
 * \param S The second argument.
 * \return Returns TRUE on success and FALSE on error.
 */
Kure_success	kure_right_residue (KureRel * rop, const KureRel * R, const KureRel * S);

/*!
 * Computes the symmetric quotient of R and S, that is:
 * \code
 * rop := syq(R,S) = (R\S) & (R^/S^) = -(R^*-S) & -(-R^*S)
 * \endcode
 *
 * As you can see from the code above, the relations R and S need to have
 * compatible dimensions.
 *
 * \param rop The relation to set. Has to be initialized.
 * \param R The first argument.
 * \param S The second argument.
 * \return Returns TRUE on success and FALSE on error.
 */
Kure_success	kure_symm_quotient (KureRel * rop, const KureRel * R, const KureRel * S);

/*!
 * @}
 */

/*!
 * \defgroup Tests Tests
 *
 *
 *
 * @{
 */

/*!
 * Tests if a given line is complete. The line is defined using a single point
 * (x,y). In contrast to most other functions, the column (here x) is the first
 * argument and the rows are the second one. The direction is passed as a
 * \ref KureDirection.
 *
 * For instance, the following code tests the vertical line at column 5.
 * \code
 * Kure_success success;
 * kure_is_row_complete_si(R, 5, 3, KURE_DIR_DOWN_UP, &success);
 * if ( !success) { ... }
 * else { ... }
 * \endcode
 *
 * \param R the relation to test.
 * \param x The column (0-indexed).
 * \param y The row (0-indexed).
 * \param dir The line type.
 * \param [out] psuccess Returns TRUE on success and FALSE on error. May be NULL.
 * \return Returns TRUE if each bit one the line is set, FALSE other wise.
 */
Kure_bool		kure_is_row_complete_si (const KureRel * R, int x, int y, KureDirection dir, Kure_success * psuccess);

/*!
 * Tests if the given relation is empty, that is, R is equal to the empty set.
 *
 * \param R the relation to test.
 * \param [out] psuccess Returns TRUE on success and FALSE on error. May be NULL.
 * \return Returns TRUE if the relation is empty, FALSE otherwise.
 */
Kure_bool		kure_is_empty (const KureRel * R, Kure_success * psuccess);

/*!
 * Tests if the given relation is univalent, that is, for each x,y,z we have
 * \f$R_{x,y}\f$ and \f$R_{x,z}\f$ implies \f$y=z\f$.
 *
 * \param R the relation to test.
 * \param [out] psuccess Returns TRUE on success and FALSE on error. May be NULL.
 * \return Returns TRUE if the relation is univalent, FALSE otherwise.
 */
Kure_bool		kure_is_univalent (const KureRel * R, Kure_success * psuccess);

/*!
 * Tests if the given relation is a vector, that is, either is row is empty or
 * filled completely.
 *
 * \note This test is slow if the relation has more than one column.
 *
 * \param R the relation to test.
 * \param [out] psuccess Returns TRUE on success and FALSE on error. May be NULL.
 * \return Returns TRUE if the relation is a vector, FALSE otherwise.
 */
Kure_bool		kure_is_vector (const KureRel * R, Kure_success * psuccess);

/*!
 * Tests if the given relation is homogeneous, that is, it has as many rows as
 * it has columns.
 *
 * \param R the relation to test.
 * \param [out] psuccess Returns TRUE on success and FALSE on error. May be NULL.
 * \return Returns TRUE if the relation is homogeneous and FALSE otherwise.
 */
Kure_bool		kure_is_hom (const KureRel * R, Kure_success * psuccess);

/*!
 * @}
 */

/*!
 * \defgroup Comparison Comparison
 *
 *
 *
 * @{
 */

/*!
 * Tests if the two relations are equal. This test is in O(1).
 *
 * \param R The first relation.
 * \param S The second relation.
 * \param [out] psuccess Returns TRUE on success and FALSE on error. May be NULL.
 * \return Return TRUE is the relations are equal, FALSE otherwise.
 */
Kure_bool		kure_equals (const KureRel * R, const KureRel * S, Kure_success * psuccess);

/*!
 * Tests if one relation is a subset of another.
 *
 * \note Tests if R is a subset of S and NOT if R includes S.
 *
 * \param R The first relation.
 * \param S The second relation.
 * \param [out] psuccess Returns TRUE on success and FALSE on error. May be NULL.
 * \return Return TRUE iff R is a subset or equal to S.
 */
Kure_bool		kure_includes (const KureRel * R, const KureRel * S, Kure_success * psuccess);

/*!
 * @}
 */


/*!
 * \defgroup Vectors
 *
 *
 *
 * @{
 */

/*!
 * Sets v to the vector where the first row is set and all other rows are
 * unset.
 *
 * \param v The relation. Don't have to be a vector.
 * \return TRUE on success and FALSE on error.
 */
Kure_success	kure_vec_begin (KureRel * v);

/*!
 * Sets v to the vector where the first row is set and all other rows are
 * unset. The vector will have the given number of rows and columns.
 *
 * \param v The relation. Don't have to be a vector.
 * \return TRUE on success and FALSE on error.
 */
Kure_success	kure_vec_begin_full (KureRel * v, mpz_t rows, mpz_t cols);

/*!
 * Same a \ref kure_vec_begin_full, except that it uses integer numbers.
 */
Kure_success	kure_vec_begin_full_si (KureRel * v, int rows, int cols);


/*!
 * Sets v to the successor of the given argument. If arg is a point and the
 * i-th row is set than v will be a point with the (i+1)-th row set. If there
 * is no such row v is set to the empty relation.
 *
 * \param v The relation. Don't have to a vector. Will have the same dimension
 *          than arg afterwards.
 * \param arg A relational point.
 * \return TRUE on success and FALSE on error.
 */
Kure_success	kure_vec_next (KureRel * v, KureRel * arg);


/*!
 * Given a vector arg a vector w is returned s.t. exactly one line in w is filled
 * which is also filled in v. The input v must be non-empty.
 *
 * \param rop Returned vector. Must be initialized. Will have the same dimension
 *            than arg is the function succeeded. Equal arguments for rop and arg
 *            are allowed.
 * \param arg Non-empty vector.
 * \return TRUE on success and FALSE otherwise. FALSE is also returned if v is
 *         empty.
 */
Kure_success	kure_vec_point (KureRel * rop, const KureRel * arg);

Kure_success 	kure_atom (KureRel*, const KureRel * arg);
Kure_success	kure_vec_inj (KureRel*, const KureRel * arg);

/*!
 * @}
 */


// Direct product, direct sums and tupling

/*!
 * \defgroup Direct Products, Directs Sums and Tupling
 *
 *
 *
 * @{
 */

/*!
 * Same as \ref kure_right_tupling, that is, \f$[R,S]=[R,S\rrbracket\f$.
 *
 * \see kure_right_tupling
 * \see kure_left_tupling
 */
Kure_success	kure_tupling (KureRel * rop, const KureRel * arg1, const KureRel * arg2);

/*!
 * Returns the right tupling of two relations. The argument rop is set as follows:
 * \f[
 * [arg1,arg2\rrbracket_{a,(x,y)} \iff  arg1_{a,x}\land arg2_{a,y}\,.
 * \f]
 * Is computed using the formula:
 * \f[
 * [R,S\rrbracket = R \pi^t \cap S \rho^t\,.
 * \f]
 *
 * \pre arg1 and arg2 need to have to same number of rows.
 *
 * \note Same as \ref kure_tupling.
 *
 * \see kure_tupling
 * \see kure_left_tupling
 *
 * \param rop The relation to set. Has rows(arg1)=rows(arg2) rows and
 *            cols(arg1)*cols(arg2) columns afterwards. Has to be initialized.
 * \param arg1 The first argument.
 * \param arg2 The second argument.
 * \return TRUE on success and FALSE on error.
 */
Kure_success	kure_right_tupling (KureRel * rop, const KureRel * arg1, const KureRel * arg2);

/*!
 * Returns the left tupling of two relations. The argument rop is set as follows:
 * \f[
 * \llbracket arg1,arg2]_{(a,b),x} \iff  arg1_{a,x}\land arg2_{b,x}\,.
 * \f]
 * Is computed using the formula:
 * \f[
 * \llbracket R,S] = \pi R \cap \rho S\,.
 * \f]
 *
 * \pre arg1 and arg2 need to have to same number of columns.
 *
 * \see kure_right_tupling
 *
 * \param rop The relation to set. Has rows(arg1)*rows(arg2) rows and
 *            cols(arg1)=cols(arg2) columns afterwards. Has to be initialized.
 * \param arg1 The first argument.
 * \param arg2 The second argument.
 * \return TRUE on success and FALSE on error.
 */
Kure_success	kure_left_tupling (KureRel * rop, const KureRel * arg1, const KureRel * arg2);

/*!
 * Returns the sum of two relations. The relations arg1 and arg2 need to have
 * the same number of columns. The result contains arg1 at the top and arg2
 * below.
 *
 * \param rop The relation to set. Has cols(arg1)=cols(arg2) columns and
 *            rows(arg1)+rows(arg2) rows afterwards. Has to be initialized.
 * \param arg1 The first argument.
 * \param arg2 The second argument.
 * \return TRUE on success and FALSE on error.
 */
Kure_success	kure_direct_sum (KureRel * rop, const KureRel * arg1, const KureRel * arg2);

/*!
 * Creates the first projection for the given domains of size m and n.
 *
 * \see kure_proj_1_si
 * \see kure_proj_1_dom
 * \see kure_proj_2
 *
 * \param rop The relation to set. Will have m*n rows and m columns. Has to
 *            be initialized.
 * \param m The size of the first domain.
 * \param n The size of the second domain.
 * \return TRUE on success and FALSE on error.
 */
Kure_success	kure_proj_1 (KureRel * R, mpz_t m, mpz_t n);

/*!
 * Same as \ref kure_proj_1, except that here the domain sizes are passed as
 * integers.
 */
Kure_success	kure_proj_1_si (KureRel*, int m, int n);

/*!
 * Similar to \ref kure_proj_1, except that here the domain is passed as an
 * object.
 */
Kure_success	kure_proj_1_dom (KureRel*, const KureDom * dom);

/*!
 * Creates the second projection for the given domains of size m and n.
 *
 * \see kure_proj_2_si
 * \see kure_proj_2_dom
 * \see kure_proj_1
 *
 * \param rop The relation to set. Will have m*n rows and n columns. Has to
 *            be initialized.
 * \param m The size of the first domain.
 * \param n The size of the second domain.
 * \return TRUE on success and FALSE on error.
 */
Kure_success	kure_proj_2 (KureRel*, mpz_t m, mpz_t n);

/*!
 * Same as \ref kure_proj_2, except that here the domain sizes are passed as
 * integers.
 */
Kure_success	kure_proj_2_si (KureRel*, int m, int n);

/*!
 * Similar to \ref kure_proj_2, except that here the domain is passed as an
 * object.
 */
Kure_success	kure_proj_2_dom (KureRel*, const KureDom * dom);

/*!
 * Create the first injection relation for the direct sum and domains of the
 * given size.
 *
 * \see kure_inj_1_si
 * \see kure_inj_1_dom
 * \see kure_inj_2
 *
 * \param rop The relation to set. Will have m rows and m+n columns. Has to
 *            be initialized.
 * \param m The size of the first domain.
 * \param n The size of the second domain.
 * \return TRUE on success and FALSE on error.
 */
Kure_success	kure_inj_1 (KureRel*, mpz_t m, mpz_t n);

/*!
 * Same as \ref kure_inj_1, except that here the domain sizes are passed as
 * integers.
 */
Kure_success	kure_inj_1_si (KureRel*, int m, int n);

/*!
 * Similar to \ref kure_inj_1, except that here the domain is passed as an
 * object.
 */
Kure_success	kure_inj_1_dom (KureRel*, const KureDom * dom);

/*!
 * Create the second injection relation for the direct sum and domains of the
 * given size.
 *
 * \see kure_inj_2_si
 * \see kure_inj_2_dom
 * \see kure_inj_1
 *
 * \param rop The relation to set. Will have n rows and m+n columns. Has to
 *            be initialized.
 * \param m The size of the first domain.
 * \param n The size of the second domain.
 * \return TRUE on success and FALSE on error.
 */
Kure_success	kure_inj_2 (KureRel*, mpz_t m, mpz_t n);

/*!
 * Same as \ref kure_inj_2, except that here the domain sizes are passed as
 * integers.
 */
Kure_success	kure_inj_2_si (KureRel*, int m, int n);

/*!
 * Similar to \ref kure_inj_2, except that here the domain is passed as an
 * object.
 */
Kure_success	kure_inj_2_dom (KureRel*, const KureDom * dom);

/*!
 * Creates the product order relation, that is,
 * \f$ \pi\cdot arg1\cdot \pi^{T} \land \rho\cdot arg2\cdot \rho^T \f$ where
 * \f$\pi,\rho\f$ are the projection relations of arg1 and arg2 respectively.
 *
 * \see kure_sum_order
 *
 * \param rop The relation to set. Has to be initialized.
 * \param arg1 First relation.
 * \param arg2 Second relation.
 */
Kure_success	kure_product_order (KureRel*, const KureRel * arg1, const KureRel * arg2);

/*!
 * Creates the sum order relation, that is,
 * \f$ \iota^{T}\cdot arg1\cdot \iota \land \kappa^{T}\cdot arg2\cdot \iota \f$ where
 * \f$\iota,\kappa\f$ are the injections relations of arg1 and arg2 respectively.
 *
 * \see kure_product_order
 *
 * \param rop The relation to set. Has to be initialized.
 * \param arg1 First relation.
 * \param arg2 Second relation.
 */
Kure_success	kure_sum_order (KureRel*, const KureRel * arg1, const KureRel * arg2);

/*!
 * @}
 */


/*!
 * \defgroup MinMaxElems Minimal and Maximal Elements
 *
 *
 *
 * @{
 */

/*!
 * Computes the minimum elements in the vector arg w.r.t. to the is-subset-of
 * relation. The vector has to represent an up-set for this to work. If this
 * is not the case or you are unsure, use \ref kure_minsets instead.
 *
 * \see kure_minsets
 * \see kure_maxsets_downset
 * \see kure_maxsets
 *
 * \param rop The relation to set. Has to be initialized.
 * \param arg The input vector.
 *
 * \note Currently, this function is limited to have just a single column.
 */
Kure_success kure_minsets_upset (KureRel * rop, const KureRel * arg);

/*!
 * Computes the minimum elements in the vector arg w.r.t. to the is-subset-of
 * relation.
 *
 * \note If you know that the vector represents an up-set, then you can use
 * the more efficient \ref kure_minsets_upset instead.
 *
 * \see kure_minsets_upset
 * \see kure_maxsets_downset
 * \see kure_maxsets
 *
 * \param rop The relation to set. Has to be initialized.
 * \param arg The input vector.
 *
 * \note Currently, this function is limited to have just a single column.
 */
Kure_success kure_minsets (KureRel * rop, const KureRel * arg);

/*!
 * Computes the maximum elements in the vector arg w.r.t. to the is-subset-of
 * relation. The vector has to represent a down-set for this to work. If this
 * is not the case or you are unsure, use \ref kure_maxsets instead.
 *
 * \see kure_minsets
 * \see kure_minsets_upset
 * \see kure_maxsets
 *
 * \param rop The relation to set. Has to be initialized.
 * \param arg The input vector.
 *
 * \note Currently, this function is limited to have just a single column.
 */
Kure_success kure_maxsets_downset (KureRel * rop, const KureRel * arg);

/*!
 * Computes the maximum elements in the vector arg w.r.t. to the is-subset-of
 * relation.
 *
 * \note If you know that the vector represents a down-set, then you can use
 * the more efficient \ref kure_maxsets_downset instead.
 *
 * \param rop The relation to set. Has to be initialized.
 * \param arg The input vector.
 *
 * \note Currently, this function is limited to have just a single column.
 */
Kure_success kure_maxsets (KureRel * rop, const KureRel * arg);

/*!
 * @}
 */

/*!
 * @}
 */

/*******************************************************************************
 *                              Domains and Types                              *
 *                                                                             *
 ******************************************************************************/

/*!
 * \addtogroup DomainsTypes
 * @{
 */

/*!
 * Creates a new direct product domain where the first type has size n
 * and the second has size m.
 *
 * \param n Must be at least 1.
 * \param m Must be at least 1.
 * \return Returns a newly created domain object.
 */
KureDom * 		kure_direct_product_new (mpz_t n, mpz_t m);

/*!
 * Same as \ref kure_direct_product_new, except that it expects integers.
 */
KureDom * 		kure_direct_product_new_si (int n, int m);

/*!
 * Creates a new direct sum domain where the first type has size n
 * and the second has size m.
 *
 * \param n Must be at least 1.
 * \param m Must be at least 1.
 * \return Returns a newly created domain object.
 */
KureDom * 		kure_direct_sum_new (mpz_t n, mpz_t m);

/*!
 * Same as \ref kure_direct_sum_new, except that it expects integers.
 */
KureDom * 		kure_direct_sum_new_si (int n, int m);

/*!
 * Create a copy of the given domain.
 *
 * \return The newly created copy.
 */
KureDom *		kure_dom_new_copy (const KureDom * self);

/*!
 * Destroys a given domain.
 *
 * \param self The domain to destroy.
 */
void  			kure_dom_destroy (KureDom * self);

/*!
 * Returns the name of the given domains.
 *
 * \return The name which is an internal string and must not be freed
 *         or changed.
 */
const char * 	kure_dom_get_name (const KureDom * self);

/*!
 * Returns the number sof components of the given domain. This is usually two.
 *
 * \return Returns the number of components and -1 if self is invalid,
 *         e.g. NULL.
 */
int kure_dom_get_comp_count (const KureDom * self);

/*!
 * Returns the size of the i-th component of the given domain. The parameter
 * size has to be initialized. Returns TRUE if the size is returned and FALSE
 * in case of an error, e.g. i is an invalid compoenent of self is NULL.
 *
 * \see kure_dom_get_comp_count
 *
 * \param size The size is stored here. Unchanged in case of an error. Has to
 *             be initialized.
 * \param i The index of the component. (0-indexed)
 * \return TRUE on success, FALSE otherwise.
 */
Kure_bool kure_dom_get_comp (const KureDom * self, mpz_t size, int i);

/*!
 * @}
 */

/*******************************************************************************
 *                                  Language                                   *
 *                                                                             *
 ******************************************************************************/

/*!
 * \addtogroup LuaAndLanguage Lua Binding and the RelView Programming Language
 *
 * The Kure2 library has a Lua binding and an own programming language. Lua
 * binding means that one can create a Lua state and call the Kure2 library
 * calls from within Lua. E.g.:
 *
 * \code
 * KureContext * c = ...;
 * lua_State * L = kure_lua_new(c);
 * KureRel * R = kure_lua_exec (L, "kure.random(kure.new_si(__c,4,4), 0.23)");
 * ...
 * kure_lua_destroy(L);
 * \endcode
 *
 * This can be very convenient in many cases because Lua is a programming
 * language with many useful features. See \ref kure_lua_new for details.
 *
 * Kure2 also has an own programming language which is originated from the
 * RelView tool. In contrast to Lua this language is very limited, but offers
 * a much simpler syntax for operations on relations like the residue
 * (e.g. R\S) or the transposition. The language is not interpreted directly.
 * Instead, it is translated into Lua code and interpreted as such.
 *
 * Functions regarding the Lua start with \e kure_lua. Functions regarding the
 * RelView programming language start with \e kure_lang.
 *
 * \code
 * KureContext * c = ...;
 * KureError * err = NULL;
 * lua_State * L = kure_lua_new(c);
 * ...
 * KureRel * R = kure_lang_exec (L, "[K,S] * ((-O)+S^)", &err);
 * ...
 * kure_lua_destroy(L);
 * \endcode
 *
 * The RelView language offers the possibility to define so called functions
 * and programs. These are also translated into Lua code while the name remains
 * the same.
 *
 * @{
 */

/*!
 * Creates a new \ref lua_State which can be used with the Kure-Lua binding
 * and the embedded language afterwards. The returned \ref lua_State has the
 * following properties:
 *
 *   - luaL_openlibs was called.
 *   - The given context is used to set the global Lua variable '__c'. This
 *     variable has a special measing for the embedded language and should not
 *     be changed! The \ref lua_State can't be used after the given
 *     \ref KureContext was destroyed. The \ref lua_State holds a reference of
 *     the context (see kure_context_ref).
 *   - The Kure library binding is added. (kure.*)
 *   - Some compatibilty functions are loaded which are used internally with
 *     the embedded language. (kure.compat.*)
 *
 * \see kure_lua_destroy, kua_lua_exec, kure_lang_exec, kure_lua_load,
 *      kure_lua_load_file
 */
lua_State *  	kure_lua_new (KureContext * context);

void 		 	kure_lua_destroy (lua_State * L);
KureRel*	 	kure_lua_exec (lua_State * L, const char * chunk, KureError ** perr);

/*!
 * Creates a global Lua variable which points to a copy of the given relation.
 * In the special case where the given relation has the name $, it is replaced
 * by __dollar. This is a convention to be compatible with RelView. This also
 * holds for \ref kure_lua_get_rel and $.
 *
 * \see kure_lua_get_rel.
 */
Kure_success 	kure_lua_set_rel_copy (lua_State * L, const char * name, KureRel * rel);
KureRel *		kure_lua_get_rel_copy (lua_State * L, const char * name);
Kure_success 	kure_lua_set_dom_copy (lua_State * L, const char * name, KureDom * dom);
KureContext * 	kure_lua_get_context (lua_State * L);
KureRel * 		kure_lua_torel (lua_State * L, int pos, KureError ** perr);
Kure_bool		kure_lua_isrel (lua_State * L, int pos);
KureDom * 		kure_lua_todom (lua_State * L, int pos, KureError ** perr);
Kure_bool		kure_lua_isdom (lua_State * L, int pos);

/*!
 * Sets the current global \ref KureContext. The context is used in operations
 * where no context is passed explicitly and where it cannot derived from
 * operands. This is mainly the case in the domain-specific embedded language.
 */
void 			kure_lua_set_global_context (lua_State * L, KureContext * c);

char * 			kure_lang_to_lua(const char * transl_unit, KureError ** perr);
char * 			kure_lang_expr_to_lua (const char * expr, KureError ** perr);

/*!
 * Executes the given expression in the embedded language and returns the
 * resulting relation. Expressions are built from '|','&','[.,.]','(',')', etc.
 * and functions calls. Neither assignments nor definitions of other objects
 * (functions, programs) are allowed. Expressions always return a relation if
 * successful. Otherwise, NULL is returned and *perr is set if perr is
 * different from NULL.
 *
 * \param L A \ref lua_State returned from \ref kure_lua_new. Remark: Some
 *          functions in the embedded language depend on a special global
 *          variable '__c' which holds a \ref KureContext.
 * \param An expression in the embedded language. E.g. "R|S" or
 *        [T+random10(R),X]".
 * \param perr Stores a description of the error if NULL is returned. May be
 *             NULL.
 * \return Returns a relation which has to be destroyed using
 * 	       \ref kure_rel_destroy.
 */
KureRel *	 	kure_lang_exec (lua_State * L, const char * expr, KureError ** perr);

/*!
 * Similar to \ref kure_lang_exec, but stores the result in a global Lua
 * variable of the given name. If the global variable is non-NULL it is
 * overwritten. If the call fails, the global Lua variable remains unchanged.
 *
 * \see kure_lang_exec
 *
 * \param var The name of the global Lua variable.
 */
Kure_success 	kure_lang_assign (lua_State * L, const char * var,
								  const char * expr, KureError ** perr);

typedef struct _KureParserObserver
{
	Kure_bool (*onFunction) (void*, const char * code, const char * lua_code);
	Kure_bool (*onProgram)  (void*, const char * code, const char * lua_code);

	void * object;
} KureParserObserver;

/*!
 * Parses the given translation unit. Each time a function or program is parsed
 * the corresponding callback function (if non-NULL) is called and provided the
 * original code as well as the generated Lua code.
 *
 * If any callback returns FALSE, the parsing stops immediately as if an error
 * occurred.
 *
 * \see KureParserObserver
 */
Kure_success 	kure_lang_parse (const char * transl_unit,
		KureParserObserver * o, KureError ** perr);

Kure_success 	kure_lang_parse_file (const char * file,
		KureParserObserver * o, KureError ** perr);

/*!
 * Loads a given translation unit written in the embedded language into the
 * given \ref lua_State which has to be created using \ref kure_lua_new.
 * Translations units may only contain function and program definitions.
 */
Kure_success	kure_lang_load (lua_State * L, const char * transl_unit, KureError ** perr);

/*!
 * Loads the contents of the given file using \ref kure_lang_load_file.
 *
 * \see kure_lang_load
 */
Kure_success	kure_lang_load_file (lua_State * L, const char * file, KureError ** perr);

/*!
 * Sets the current callback function which is called if an assertion fails.
 * The function is directly called from within Lua and has to take the
 * assertions name as first argument.
 */
void 			kure_lang_set_assert_func (lua_State * L, int(*)(lua_State*));


/*!
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* KURE_H_ */
