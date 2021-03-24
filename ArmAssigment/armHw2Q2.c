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

 
	ldr	r0, =lcm	// Load LCM title	
	bl      kprintf         // Print LCM title
	bl 	LCM		// Call function LCM

	/* End of your program.         */
        /* Function epilogue.           */
        /* Restores the environment from the O/S. */
done:
        add	sp, sp, #32	/* Destroy stack frame.     */
                                /* Pop O/S state from stack.*/
	pop	{r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}
        bx      lr              /* Return to O/S.           */



LCM:
	push    {r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}	
	b	loopCheck1	// Go to loopCheck1

loopCheck1:
	bl	getInt 		// r0 = user input, r1 = possible stop token
	cmp 	r1, #1 		// Check for stop token
	beq 	done		// If true, end  program
	
	mov 	r4, r0		// Store user value in r4
	mov 	r10, r0		// Create backup in r10 for user input
	cmp 	r0, #0		// Compare user input with zero  
	blt 	loopCheck1	// Disreguard input if neg int, and try again

	b 	loopCheck2 	// Go to loopCheck2
	
loopCheck2: 
	bl 	getInt		// r0 = user input, r1 = possible stop token
	cmp     r1, #1          // Check for stop token
        beq     done		// If true, end program
	
	mov 	r5, r0		// Get second user input
	mov 	r6, r0 		// Create a backup in r6 for user input
	cmp 	r0, #0		// Compare user input with zero
	blt 	loopCheck2 	// Disreguard input if neg int, and try again

	b	calc 		// Else go to calc
	
calc:		
	mov     r0, r4          // Set r0 to (first int)
        cmp     r0, #0          // Compare (first int) and 0
        beq     zero            // Go to zero if first int = 0

	mov     r0, r5          // Set r0 to (second int)
        cmp     r0, #0          // Compare (second int) and 0
        beq     zero            // Go to zero if second int = 0

	mov 	r0, r4		// First int = Dividend
	mov 	r1, r5		// Second int = Divisor	

	bl 	divide		// Divide
	cmp 	r1, #0		// Compare to see if there is a remainder
	beq	dividedByCase	// If not, go to dividedByCase

	add 	r4, r4, r10 	// (New r4 val) = (Previous r4 val) + (Original first int)

	b 	calc		// Go to begeining of calc
	
zero:
	mov 	r1, r10		// kprintf first output = (first num)
	mov 	r2, r5		// kprintf second output = (second num)
	mov 	r7, #0		// kprintf third output	= (0)
	
	b 	output		// Go to ouput 

dividedByCase:
	mov 	r1, r10 	// kprintf first output = (first num)
	mov 	r1, r5		// kprintf second  output = (second num)
	mov 	r7, r4		// kprintf third output = (LCM)
	b 	output		// Go to output

output: 	
	mov 	r1, r10		// Set up r1, r2, and r3 for output format
	mov 	r2, r5 		// Which is (first num) (second num) (LCM)
	mov 	r3, r4		
	
	ldr 	r0, =formatAns  // Load answerFormat
	bl 	kprintf		// Print answer	
	
	b 	loopCheck1	// Infinite loop until user input says to stop


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
lcm: 			.asciz "\r\nLCM Calculator\r\n"
formatAns:		.asciz "LCM of %d and %d is %d\r\n"
