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
//	Run the sampler, once.
//
static void run_sampler( void ) {
	byte	here, next;
	
	//
	//	Empty sample space.
	//
	samples_left = max_samples;
	next_sample = sample;
	last_sample = SAMPLE_INPUT;
	counter = REPEAT_DURATION;
	//
	//	Fill up the available space
	//
	noInterrupts();
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
	Serial.begin(115200);
	while( !Serial );	// wait for serial port to connect. Needed for native USB port only

	//
	//	Display banner.
	//
	Serial.println( "Digital Sampler" );
	Serial.println( "(press key to sample)" );
	//
	//	Initialis the input PORT we will be using.
	//
	SAMPLE_DIR = 0;		// All pin are input.
	SAMPLE_OUTPUT = 0;	// with no pullup.
}

//
//	Here is our event loop, effectively (for the moment)
//	simply waiting for any input for it to initialise a
//	single run of the sampler.
//
void loop( void ) {
	if( Serial.available()) {
		while( Serial.available()) (void)Serial.read();
		//
		//	Execute a sample run.
		//
		Serial.println( "Sampling!" );
		run_sampler();
	}
}

//
//	EOF
//
