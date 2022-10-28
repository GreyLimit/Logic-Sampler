/*
 *	Replay an arduino logic analyser output
 *	converting specific pin data into the
 *	I2C commands/actions that the analyser
 *	captured.
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/*
 *	Reads from standard input, writes to
 *	standard output.
 */

/*
 *	Define some geenric basic constants..
 */
#define TRUE	(0==0)
#define FALSE	(0==1)
#define BUFFER	200
#define EOS	'\0'

/*
 *	Simple bit extraction stuff.
 */
#define bit(n)	(1<<(n))

/*
 *	Break down the sample line data.
 */
static char *decompose( char *data, int *row, int *delta, int *sample ) {
	int	x;

	/*
	 *	Dumb but functional?
	 */
	x = 0;
	while( isdigit( *data )) x = x * 10 + ( *data++ - '0' );
	*row = x;
	while( isspace( *data )) data++;
	x = 0;
	while( isdigit( *data )) x = x * 10 + ( *data++ - '0' );
	*delta = x;
	while( isspace( *data )) data++;
	x = 0;
	while( isdigit( *data )) x = x * 10 + ( *data++ - '0' );
	*sample = x;
	while( isspace( *data )) data++;
	return( data );
}


/*
 *	Here we go...
 */
int main( int argc, char *argv[]) {
	char		buffer[ BUFFER ],	/* Data being processed */
			scratch[ BUFFER ];	/* Temporary output buffer */
	int		clk,			/* The bit which represents the Clock */
			sda,			/* The bit which represents the Data */
			last_clk,		/* The previous clock value */
			last_sda;		/* The previous data value */
	int		next,			/* Next bit to collect 8..1 then 0 for nack/ack */
			value,			/* Value collected so far */
			adrs;			/* true for first data after START */

	if( argc != 3 ) {
		fprintf( stderr, "Usage: %d {clk} {sda}\n\t{???} are bit numbers (0..7).\n", argv[ 0 ]);
		return( 1 );
	}
	/*
	 *	Note where the clock and data can be found.
	 */
	clk = bit( atoi( argv[ 1 ]));
	sda = bit( atoi( argv[ 2 ]));
	/*
	 *	We start by assuming that in the first instance
	 *	both clock and data are held high (which ought
	 *	to be the case with pull-up resistors).
	 */
	last_clk = TRUE;
	last_sda = TRUE;
	adrs = FALSE;

	/*
	 *	Run through the samples line by line..
	 */
	while( fgets( buffer, BUFFER, stdin )) {
		/*
		 *	Line starts with some sort of digit?
		 */
		if( isdigit( *buffer )) {
			int	r,	/* Sample number */
				d,	/* Delta samples from previous output sample */
				s;	/* This sample value */
			char	*a;	/* ASCII smaple trace (decorative only) */

			int	this_clk, this_sda,
				clk_up, clk_down,
				sda_up, sda_down;
			char	*m;
			
			/*
			 *	Breakdown the input text
			 */
			buffer[ strlen( buffer )-1 ] = EOS;
			a = decompose( buffer, &r, &d, &s );
			/*
			 *	Analyse the I2C changes; clock then data.
			 */
			this_clk = (( s & clk ) != 0 );
			if( last_clk ) {
				clk_up = FALSE;
				clk_down = !this_clk;
			}
			else {
				clk_down = FALSE;
				clk_up = this_clk;
			}
			this_sda = (( s & sda ) != 0 );
			if( last_sda ) {
				sda_up = FALSE;
				sda_down = !this_sda;
			}
			else {
				sda_down = FALSE;
				sda_up = this_sda;
			}
			/*
			 * 	Break down the I2C logic into the various
			 *	signal patterns
			 */
			m ="";
			
			/*
			 *	The START condition
			 *	===================
			 *
			 *	This is where the clock stays high and the data line drops.
			 */
			if( sda_down && this_clk && last_clk ) {
				value = 0;
				next = 8;
				m = "START";
				adrs = TRUE;
			}

			/*
			 *	DATA / NACK / ACK bit
			 *	=====================
			 *
			 *	This is where the clock goes high and the data line has
			 *	remained either high or low over the transition.
			 */
			if( clk_up ) {
				if( this_sda && last_sda ) {
					/*
					 *	'1'
					 */
					if( next ) {
						/*
						 *	Collecting 1 data.
						 */
						next--;
						value |= bit( next );
						m = "1";
					}
					else {
						/*
						 *	Not Acknowledgement.
						 */
						if( adrs ) {
							sprintf( scratch, "Nack adrs:%d %s", ( value >> 1 ), (( value & 1 )? "Read": "Write" ));
						}
						else {
							sprintf( scratch, "Nack data:%d", value );
						}
						m = scratch;
					}
				}
				if( !this_sda && !last_sda ) {
					/*
					 *	'0'
					 */
					if( next ) {
						/*
						 *	Collecting 0 data.
						 */
						next--;
						m = "0";
					}
					else {
						/*
						 *	Acknowledgement.
						 */
						if( adrs ) {
							sprintf( scratch, "Ack adrs:%d %s", ( value >> 1 ), (( value & 1 )? "Read": "Write" ));
						}
						else {
							sprintf( scratch, "Ack data:%d", value );
						}
						m = scratch;
						value = 0;
						next = 8;
						adrs = FALSE;
					}
				}
			}

			/*
			 *	The STOP condition.
			 *	===================
			 *
			 *	This is when sda goes high while the clock remained high.
			 */
			if( sda_up && this_clk && last_clk ) {
				m = "STOP";
			}

			/*
			 *	Output anotated line.
			 */
			printf( "%d\t%d\t%d\t%s\t%s\n", r, d, s, a, m );
			
			last_clk = this_clk;
			last_sda = this_sda;
		}
		else {
			/*
			 *	Any line that does not start with a digit
			 *	is simply output the stdout unmodified.
			 */
			fputs( buffer, stdout );
		}
	}
	return( 0 );
}

/*
 *	EOF
 */
