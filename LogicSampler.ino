/*
LogicSampler: An Arduino Sketch to capture binary port data
Copyright (C) 2022  Jeff Penfold

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

//
//	A simple (hopefully fast) sketch to convert
//	an Ardiuno into a simple Logic Capture device.
//

//
//	I am, without apology, writing this for the
//	mega 2560.  It simply has enough memory to
//	be useful.
//

//
//	These are the macros which outline which
//	port we will be accessing.
//
//		DDR_	Set pin direction 
//		PORT_
//		PIN_
//
#define SAMPLE_DIR	DDRK
#define SAMPLE_OUTPUT	PORTK
#define SAMPLE_INPUT	PINK

//
//	Define the counter start for duplicate entries to be
//	placed into the sample store
//
#define REPEAT_DURATION	250

//
//	Define a simple structure to capture a single
//	snap shot of the inputs along with the delta
//	time from the previous snap shot.
//
typedef struct {
	byte		data,
			delta;
} snap;

//
//	Define the maximum number of snap transitions
//	that we will capture.
//
static const word	max_samples = 3000;

//
//	Here is a scratch space, and how many we have used.
//
static snap		sample[ max_samples ],
			*next_sample;
static word		samples_left;

//
//	The last and current samples as global variables
//	to reduces access time.
//
static byte		last_sample,
			this_sample,
			counter;

//
//	Declare an 'input buffer' used to capture the dynamically
//	input 'trigger' pin.  
//
static int	trigger;
static bool	trigger_high;

//
//	Run the sampler, once.
//
static void run_sampler( void ) {
	byte	here, next;
	
	//
	//	Empty sample space.
	//
	samples_left = max_samples;
	next_sample = sample;
	counter = REPEAT_DURATION;
	//
	//	Fill up the available space
	//
	noInterrupts();
	if(( trigger >= 0 )&&( trigger <= 7 )) {
		int	b;
		
		b = 1 << trigger;
		if( trigger_high ) {
			while(( last_sample = SAMPLE_INPUT & b ));
			while(!( this_sample = SAMPLE_INPUT & b )) {
				last_sample = this_sample;
			}
		}
		else {
			while(!( last_sample = SAMPLE_INPUT & b ));
			while(( this_sample = SAMPLE_INPUT & b )) {
				last_sample = this_sample;
			}
		}
		next_sample->delta = REPEAT_DURATION;
		next_sample->data = last_sample;
		next_sample++;
		samples_left--;
		next_sample->delta = REPEAT_DURATION;
		next_sample->data = this_sample;
		next_sample++;
		samples_left--;
		last_sample = this_sample;
		counter = REPEAT_DURATION;
	}
	else {
		last_sample = SAMPLE_INPUT;
	}
	while( samples_left ) {
		if((( this_sample = SAMPLE_INPUT ) != last_sample )||( !counter )) {
			next_sample->delta = counter;
			next_sample->data = this_sample;
			last_sample = this_sample;
			counter = REPEAT_DURATION;
			next_sample++;
			samples_left--;
		}
		else {
			counter--;
		}
	}
	interrupts();
	//
	//	Now we dump all the sample to the output
	//
	Serial.println();
	Serial.print( "index\tdelta\tsample" );
	for( int b = 8; b; b-- ) {
		Serial.print( "\t" );
		Serial.print( b-1 );
	}
	Serial.println();
	for( word i = 0; i < max_samples; i++ ) {
		here = sample[ i ].data;
		if( i < ( max_samples - 1 )) next = sample[ i + 1 ].data;
		Serial.print( i );
		Serial.print( "\t" );
		Serial.print( REPEAT_DURATION - sample[ i ].delta );
		Serial.print( "\t" );
		Serial.print( here );
		for( int b = 0x80; b; b >>= 1 ) {
			Serial.print( "\t" );
			if(( here & b ) == ( next & b )) {
				/*
				 *	Unchanged signal
				 */
				if( here & b ) {
					Serial.print( ". |" );
				}
				else {
					Serial.print( "| ." );
				}
			}
			else {
				/*
				 *	Changed signal
				 */
				if( here & b ) {
					Serial.print( "._|" );
				}
				else {
					Serial.print( "|_." );
				}
			}
		}
		Serial.println();
	}
}

//
//	Set up the console.
//
void setup( void ) {
	//
	//	Initialize serial and wait for port to open:
	//
	Serial.begin( 115200 );
	while( !Serial );	// wait for serial port to connect. Needed for native USB port only

	//
	//	Display banner.
	//
	Serial.println( "Digital Sampler" );
	Serial.println( "Keys:-" );
	Serial.println( "0-7\tSelect trigger pin" );
	Serial.println( "h\tTrigger when pin goes high" );
	Serial.println( "l\tTrigger when pin goes low" );
	Serial.println( "[CR]\tStart sampling" );
	//
	//	Initialise the input PORT we will be using.
	//
	SAMPLE_DIR = 0;		// All pin are input.
	SAMPLE_OUTPUT = 0;	// with no pullup.
	//
	//	Set default trigger details: none.
	//
	trigger = -1;
}

//
//	Here is our event loop, effectively (for the moment)
//	simply waiting for any input for it to initialise a
//	single run of the sampler.
//
void loop( void ) {
	if( Serial.available()) {
		char	in;
		
		in = Serial.read();
		if(( in == '\n' )||( in == '\r' )) {
			//
			//	Execute a sample run.
			//
			if( trigger >= 0 ) {
				Serial.print( "Trigger on " );
				Serial.print( trigger );
				if( trigger_high ) {
					Serial.println( " -> high" );
				}
				else {
					Serial.println( " -> low" );
				}
			}
			else {
				Serial.println( "Sampling!" );
			}
			run_sampler();
		}
		else {
			if(( in == 'h' )||( in == 'l' )) {
				trigger_high = ( in == 'h' );
			}
			else {
				if(( in >= '0' )&&( in <= '7' )) {
					trigger = in - '0';
				}
				else {
					trigger = -1;
				}
			}
		}
	}
}

//
//	EOF
//
