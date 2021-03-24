/* Main program template, including getInt, for ARM Playground 3.2 */
/* Marquette University 2019, Department of Computer Science */

#include <arm.h>

/* Main program starts here.  Put nothing before _main_:.  */
.section .text
.globl _main_
_main_:
        /* Function prolog.                                  */
        /* Sets up environment for user program to execute.  */
                                /* Push O/S state on Stack.  */
	push	{r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}
        sub	sp, sp, #32	/* Make stack frame.  */
        /* Function prolog ends. */
        /* Start your program.   */

	ldr	r0, =low	// Load low title
	bl	kprintf		// Print low title
	       
	bl      getInt          // r0 = user input (low), r1 = end token
	mov 	r4, r0		// r4 = store (low) in r4
	MOV 	r7, r4 		// Store backup of r4 in r7
	cmp 	r1, #1 		// Check for end token
	beq 	done		// End program  
	
	ldr     r0, =high	// Load high title
        bl      kprintf		// Print high title

	bl 	getnum 		// r0 = user input (high), r1 = end token
	mov 	r5 , r0		// r5 = store (high) in r5
	mov 	r8, r5		// Store backup of r5 in r8
	cmp 	r1, #1 		// Check for end token
	beq 	done		// End program
	
	b 	compare		// Else, go to compare

compare:	
	cmp 	r4, #0		// Compare low and 0
	blt	less		// Go to less, if low is less than 0 
	
	cmp 	r5, #255	// Compare high and 255
	bgt	more       	// Go to more, if high is more than 255
	
	b 	loop		// Go to loop

less:
	mov 	r4, #0 		// Set low to 0
	b 	compare		// Go back to compare

more:
	mov 	r5, #255	// Set high to 255
	b 	compare 	// Go back to compare


loop: 
	mov 	r1, r4 		// Set r1 to current low
	ldr 	r0, =decimal	// Load decimal format
	bl 	kprintf		// Print in decimal format
	
	mov 	r1, r4		// r1 = int value for correct hex format
	mov 	r3, r4 		// r3 = int value for correct ascii format
	
	mov 	r0, r1		// r0 = int value
	bl 	toBinary 	// Covert r0 to binary 
	mov     r2, r0		// r2 = binary value for correct binary format
	mov  	r1, r4
	mov 	r3, r4
	

	ldr     r0, =outputForm	// Load output format
 	bl      kprintf		// Print output format

	add  	r4, #1		// Add one to low int 
	cmp 	r4, r5 		// Compare low an high ints
	bgt 	done		// Exit program if low is greater than high
	
	b 	loop		// Else, do it again!!!

	/* End of your program.         */
        /* Function epilogue.           */
        /* Restores the environment from the O/S. */
done:
        add	sp, sp, #32	/* Destroy stack frame.     */
                                /* Pop O/S state from stack.*/
	pop	{r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}
        bx      lr              /* Return to O/S.           */


#define MASK   r6
#define RESULT r5
#define REM    r4

toBinary:
	push    {r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}

	mov     MASK, #0x80
	mov     r3, #0xFF
	and     REM, r0, r3
	mov     RESULT, #0
toblp:  cmp     MASK, #0
	ble     tobdone

	mov     r3, #10
	mul     RESULT, RESULT, r3
	and     r0, REM, MASK
	cmp     r0, #0
	beq     tblp2
	add     RESULT, RESULT, #1
tblp2:  lsr     MASK, MASK, #1
	b       toblp

tobdone:
	mov     r0, RESULT
	pop     {r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}
	bx      lr


/* getInt function returns positive and negative integers */
/*  read in as ASCII characters representing base-10 values.  */
/*  Returns:    */
/*      r0 = integer */
/*      r1 = End Of File (EOF) flag, 1 (true) or 0 (false). */
#define TOTAL   r4
#define DIGIT   r5
#define SIGN    r6
#define SAW_EOF r7

getInt:
        push    {r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}

        mov     r0, #'?'        /* Print a friendly "? " prompt. */
        bl      putchar
        mov     r0, #' '
        bl      putchar
        mov     TOTAL, #0       /* Initialize variables         */
        mov     SIGN, #0
        mov     SAW_EOF, #0

        bl      getchar         /* Get first character          */
        cmp     r0, #'-'        /* IF it is a minus sign,       */
        bne     gi2             /*  THEN set SIGN = 1,          */
        mov     SIGN, #1        /*  ELSE jump into main loop.   */
        bl      putchar         /* Echo minus sign.             */

giloop:
        bl      getchar         /* Get first possible digit.    */
gi2:    cmp     r0, #4          /* If EOF, exit on special case */
				/* Note: 4 is the ASCII value   */
				/* for End of Transmission,	*/
				/* signaled by ctrl+D.		*/
        beq     gieof
        cmp     r0, #'\n'       /* If newline, then done        */
        beq     gidone          /*  reading digits.             */
        cmp     r0, #'0'        /* Check for digits between '0' */
        blt     giloop          /*  and '9'.                    */
        cmp     r0, #'9'
        bgt     giloop          /* Otherwise, ignore character  */
        mov     DIGIT, r0
        bl      putchar         /* Echo digit                   */

        sub     r0, DIGIT, #'0' /* Horner's Method to calculate */
        mov     r1, #10         /*  value of number             */
        mul     TOTAL, TOTAL, r1
        add     TOTAL, TOTAL, r0
        b       giloop

gieof:  mov     SAW_EOF, #1     /* Special case EOF flag = true */
gidone: 
        cmp     SIGN, #1        /* IF sign = 1, subtract total  */
        bne     gid2            /*  from zero to get negative   */
        mov     r1, #0
        sub     TOTAL, r1, TOTAL
gid2:
        mov     r0, #'\r'       /* Echo newline and carriage return */
        bl      putchar
        mov     r0, #'\n'
        bl      putchar
        mov     r0, TOTAL       /* Stage return values          */
        mov     r1, SAW_EOF

        pop     {r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}
        bx      lr              /* Return to caller             */


/* .data section is for non-code entities, like format strings. */
/* Use .asciz, which tells the assembler to terminate the       */
/* string with a null character (zero in ASCII)                 */
.section .data
low:			.asciz "low"
high:			.asciz "high"
decimal:		.asciz "%d"
outputForm: 		.asciz "     0x%02X 	0b%08d	   %c \r\n"
