#include "header_files/Global.hpp"
#include "header_files/Utility.hpp"
#include <string>


// --- interpolation -- //

/* smoothstep function, interpolates from edge to edge along a quadratic akin to a sine curve with transformations applieed */
float smoothstep (float x, float edge0, float edge1) {
   // Scale, and clamp x to 0..1 range
   x = clamp((x - edge0) / (edge1 - edge0));

   return x * x * (3.0f - 2.0f * x);
}

/*  limits a given values to the ranges  */
float clamp(float x, float lowerlimit, float upperlimit) {
  if (x < lowerlimit) return lowerlimit;
  if (x > upperlimit) return upperlimit;
  return x;
}

/* linearly interpolates between two values */
float lerp( float t, float a, float b )
{ return a + t*( b - a ); }


/* returns a sample of a 1 dimensional row of perlin noise */
float simpleNoise1D( int x )
{
    // the binary left shift is essentially *2^13
    x = (x<<13) ^ x;
    // & 0x7fffffff gets the absolute value
    return ( 1.0 - ( (x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

/* returns a sample of a 2 dimensional row of perlin noise */
float simpleNoise2D( int x, int y )
{
    int n = x + y * 257;
    n = (n<<13) ^ n;
    return ( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) &
    0x7fffffff) / 1073741824.0);
}

template <typename T>
T findLargest(const T* values, size_t size) {
    if (size == 0) {
        throw std::runtime_error("Cannot find largest value in an empty array");
    }

    T largest = values[0];
    for (size_t i = 1; i < size; ++i) {
        if (values[i] > largest) {
            largest = values[i];
        }
    }
    return largest;
}



float lerpNoise1D( float x )
{
    int xi;     //integer component
    float xf;     //floating point component
    float l0, l1; //uninterpolated values
    xi = (int)x;
    xf = x - (float)xi;
    l0 = simpleNoise1D( xi );
    l1 = simpleNoise1D( xi+1 );
    return lerp( xf, l0, l1 );  //interpolate the noise values
}

float lerpNoise2D( float x, float y )
{
    int xi,yi;                //integer component
    float xf,yf;              //floating point component
    float l00, l01, l10, l11; //uninterpolated values
    float l0, l1;             //partially interpolated values
    xi = (int)x;
    xf = x - (float)xi;
    yi = (int)y;
    yf = y - (float)yi;
    l00 = simpleNoise2D( xi  , yi   );
    l01 = simpleNoise2D( xi+1, yi   );
    l10 = simpleNoise2D( xi  , yi+1 );
    l11 = simpleNoise2D( xi+1, yi+1 );
    l0 = lerp( xf, l00, l01 );  //interpolate across the x axis
    l1 = lerp( xf, l10, l11 );
    return lerp( yf, l0, l1 );  //interpolate across the y axis
}



// float smoothNoise1D( float x )
// {
//     int xi;
//     float xf;
//     float xs;
//     float l0, l1; //uninterpolated values
//     //integer component
//     //floating point component
//     //s-curve function result
//     xi = (int)x;
//     xf = x - (float)xi;
//     xs = smoothstep(xf);
//     l0 = simpleNoise1D( xi );
//     l1 = simpleNoise1D( xi+1 );
//     return lerp( xf, l0, xs );  //interpolate the noise values useing the smoothstep value
// }

float smoothNoise2D( float x, float y )
{
    //integer component
    //floating point component
    //s-curve function results
    int xi,yi;
    float xf,yf;
    float xs,ys;
    float l00, l01, l10, l11; //uninterpolated values
    float l0, l1;
    xi = (int)x;
    xf = x - (float)xi;
    xs = smoothstep(xf);
    yi = (int)y;
    yf = y - (float)yi;
    ys = smoothstep(yf);
    //partially interpolated values
    l00 = simpleNoise2D( xi  , yi   );
    l01 = simpleNoise2D( xi+1, yi   );
    l10 = simpleNoise2D( xi  , yi+1 );
    l11 = simpleNoise2D( xi+1, yi+1 );
    l0 = lerp( xs, l00, l01 );  //interpolate across the x axis using the smoothstep curve
    l1 = lerp( xs, l10, l11 );
    return lerp( ys, l0, l1 );  //interpolate across the y axis using the smoothstep curve
}


// int NoElementsArrInt(int* arr) { return (sizeof(arr) / sizeof(int) );}

// float NoElementsArrFloar(float* arr) { return (sizeof(arr) / sizeof(float) );}


// int getMaxArrInt(int* arr)
// {
//     size_t size = sizeof(arr) / sizeof(arr[0]);

//     if (size == 0 || arr == nullptr) {
//         // Handle edge cases where array is empty or pointer is nullptr.
//         // You can modify this behavior based on your needs.
//         throw std::invalid_argument("Invalid input: array is empty or pointer is nullptr");
//     }

//     int largest = arr[0]; // Assume the first element is the largest

//     bool larger;
//     for (size_t i = 1; i < size; ++i) {

//         larger = (largest < arr[i]);
//         largest = largest - (largest * larger) + ( arr[i] * larger );  // branchless appreach
//     }

//     return largest;
// }

// GLuint getMaxArrGLuint(GLuint* arr)
// {
//     size_t size = sizeof(arr) / sizeof(arr[0]);

//     if (size == 0 || arr == nullptr) {
//         // Handle edge cases where array is empty or pointer is nullptr.
//         // You can modify this behavior based on your needs.
//         throw std::invalid_argument("Invalid input: array is empty or pointer is nullptr");
//     }

//     GLuint largest = arr[0]; // Assume the first element is the largest

//     bool larger;
//     for (size_t i = 1; i < size; ++i) {

//         larger  = ( largest < arr[i] );
//         largest = largest - ( largest * larger ) + ( arr[i] * larger );  // branchless appreach
//     }

//     return largest;
// }


// int getMinArrInt(int* arr)
// {
//     size_t size = sizeof(*arr) / sizeof(arr[0]);


//     if (size == 0 || arr == nullptr) {
//         // Handle edge cases where array is empty or pointer is nullptr.
//         // You can modify this behavior based on your needs.
//         throw std::invalid_argument("Invalid input: array is empty or pointer is nullptr");
//     }

//     int largest = arr[0]; // Assume the first element is the largest

//     bool larger;
//     for (size_t i = 1; i < size; ++i) {

//         larger = (largest > arr[i]);
//         largest = largest - (largest * larger) + ( arr[i] * larger );  // branchless appreach
//     }

//     return largest;
// }

float getMaxArrFloat(int* arr);

float getMinArrFloat(int* arr);




std::vector<std::string> GetFolderContents(const std::string& path)
{
    std::vector<std::string> files;

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        files.push_back(entry.path().string());
    }
    return files;
}


float GetRandomfloat(float R1 ,float R2)
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed); // Mersenne Twister pseudo-random generator

    // Define a distribution (e.g., uniform distribution between 1 and 100)
    std::uniform_real_distribution<float> distribution(R1, R2);

    // Generate a random number
    return distribution(generator);
}

int GetRandomIntieger(int R1 , int R2)
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed); // Mersenne Twister pseudo-random generator

    // Define a distribution (e.g., uniform distribution between 1 and 100)
    std::uniform_int_distribution<int> distribution(R1, R2);

    // Generate a random number
    return distribution(generator);

}



// --- data handling --- //

std::string get_working_path()
{
    char temp [ PATH_MAX ];

    if ( getcwd(temp, PATH_MAX) != 0) 
        return std::string ( temp );

    int error = errno;

    switch ( error ) {
        // EINVAL can't happen - size argument > 0

        // PATH_MAX includes the terminating nul, 
        // so ERANGE should not be returned

        case EACCES:
            throw std::runtime_error("Access denied");

        case ENOMEM:
            // I'm not sure whether this can happen or not 
            throw std::runtime_error("Insufficient storage");

        default: {
            std::ostringstream str;
            str << "Unrecognised error" << error;
            throw std::runtime_error(str.str());
        } // default
    } // switch (error)
}

std::string get_file_contents(const char* filename)
{
	// std::ifstream in(filename, std::ios::binary);
    

    // if (in)
    // {
    //     std::string contents;
    //     in.seekg(0 , std::ios::end);
    //     contents.resize(in.tellg());
    //     in.seekg(0 , std::ios::beg);
    //     in.read(&contents[0], contents.size());
    //     in.close();
    //     return (contents);
    // }

    std::ifstream in(filename, std::ios::binary);
	std::string contents = "";
	if (in)
	{
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
        std::cout << "no errors thrown gathering : " << filename << ". " << std::endl;

        // std::cout << filename << " : " << contents << std::endl;
		return(contents);
	}
    std::cerr << "Failed to retrive text from file" << " \"  " << filename << " \" " << std::endl;
    throw(errno);
};