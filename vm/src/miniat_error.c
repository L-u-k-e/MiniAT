#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "miniat/miniat.h"
#include "miniat/miniat_error.h"
#include "miniat_priv_util.h"
#include "miniat_priv_typedefs.h"
#include "miniat_priv_structures.h"

/* Start Error Messages */
#define M_ERROR_MSG_NONE                    "No Error"                         //!< Error message illegal argument
#define M_ERROR_MSG_ILLEGAL_ARGUMENT        "Illegal Argument"                 //!< Error message illegal argument
#define M_ERROR_MSG_DOUBLE_INIT             "Double Initialization of library" //!< Error message double init
#define M_ERROR_MSG_OUT_OF_RANGE            "Error Number Out of Range"        //!< Error message number out of range
#define M_ERROR_MSG_INVALID_ERROR_NUM       "Invalid Error Number"             //!< Error message invalid error number
#define M_ERROR_MSG_INVALID_BINARY          "Invalid Binary"                   //!< Error message invalid binary
#define M_ERROR_MSG_PC_QUEUE_FULL           "Program Counter Queue Full"       //!< Error message program counter queue is full and an insert was attempted
#define M_ERROR_MSG_REG_QUEUE_FULL          "Register Queue Full"              //!< Error message register hazard queue is full and an insert was attempted
#define M_ERROR_MSG_MEM_QUEUE_FULL          "Memory Queue Full"                //!< Error message memory hazard queue is full and an insert was attempted
#define M_ERROR_MSG_PC_QUEUE_EMPTY          "Program Counter Queue Empty"      //!< Error message program counter queue is empty and a remove was attempted
#define M_ERROR_MSG_REG_QUEUE_EMPTY         "Register Queue Empty"             //!< Error message register hazard queue is empty and a remove was attempted
#define M_ERROR_MSG_MEM_QUEUE_EMPTY         "Memory Queue Empty"               //!< Error message memory hazard queue is empty and a remove was attempted
#define M_ERROR_MSG_NULL_POINTER            "Null Pointer"                     //!<Error message null pointer
#define M_ERROR_MSG_BARN_FIRE               "Barn Fire"                        //!<Error message something crazy has occurred and we don't know what to do
#define M_ERROR_MSG_OUT_OF_MEM              "Out of Memory"                    //!<Error message out of memory
#define M_ERROR_MSG_ERROR_ALREADY_HANDLED   "Error has already been handled"   //!<Error message someone tried to handle an error that already has been handled
/* End Error Messages */


m_error __m_err__;
m_error __m_g_err__;
/*
 * This needs to be kept in order with the enum definitions
 */
static const char *error_map[] = {
		M_ERROR_MSG_NONE,
		M_ERROR_MSG_ILLEGAL_ARGUMENT,
		M_ERROR_MSG_DOUBLE_INIT,
		M_ERROR_MSG_OUT_OF_RANGE,
		M_ERROR_MSG_INVALID_ERROR_NUM,
		M_ERROR_MSG_INVALID_BINARY,
		M_ERROR_MSG_PC_QUEUE_FULL,
		M_ERROR_MSG_REG_QUEUE_FULL,
		M_ERROR_MSG_MEM_QUEUE_FULL,
		M_ERROR_MSG_PC_QUEUE_EMPTY,
		M_ERROR_MSG_REG_QUEUE_EMPTY,
		M_ERROR_MSG_MEM_QUEUE_EMPTY,
		M_ERROR_MSG_NULL_POINTER,
		M_ERROR_MSG_BARN_FIRE,
		M_ERROR_MSG_OUT_OF_MEM
};

void __error_push(m_error_stack *env) {

	m_error_stack **head = __m_g_err__.head ? &(__m_err__.head) : &(__m_g_err__.head);

	if(head && *head) {
		env->next = *head;
	}
	*head = env;

	return;
}

m_error_stack *__error_pop(m_error_stack_action pop) {

	m_error_stack **head = __m_err__.head ? &(__m_err__.head) : &(__m_g_err__.head);
	m_error_stack *tmp = NULL;
	if(head && *head) {
		tmp = *head;
		if(pop == m_error_stack_pop) {
			*head = (*head)->next;
		}
	}

	return tmp;
}

void miniat_dump_error() {

	m_error *err = __m_err__.head ? &(__m_err__) : &(__m_g_err__);
	if(err->curr_err.active) {

		fprintf(stderr, "Error %d -- %s in file: %s in function: %s on line %d\n",
			err->curr_err.num,
			error_map[err->curr_err.num],
			err->curr_err.file,
			err->curr_err.func,
			err->curr_err.line);

		err->curr_err.active = 0;
	}

	return;
}

void miniat_panic(int rc) {
	fprintf(stderr, "PANIC -- Uncaught Exception\n");
	miniat_dump_error();
	exit(rc);
}

void __set_active_error(int line, const char *file, const char *func, m_error_num e) {

	m_error *err = __m_err__.head ? &(__m_err__) : &(__m_g_err__);
	err->curr_err.active = 1;
	err->curr_err.line = line;
	err->curr_err.func = func;
	err->curr_err.file = file;
	err->curr_err.num = e;
	return;
}
