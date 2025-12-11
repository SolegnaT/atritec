// Assume that the C standard library is available on the platform.
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <ctime>

typedef struct {
    uint32_t scan_number;
    float x_angle_deg;
    float y_angle_deg;
    float distance_m;
    uint16_t intensity;
} AtrisenseRecord;
int atrisense_record_sz = 18;
#define SIZEOF_ATRISENSERECORD 18

typedef struct {
    uint32_t scan_number;
    float x;
    float y;
    float z;
    uint16_t intensity;
} Point;

int point_sz = 18;

#define BUFFER_SIZE 256 // Should be tuned for performance via profiling.
// Using globals to avoid using malloc/free, this simplifies future refactoring.
void* input_buffer[SIZEOF_ATRISENSERECORD*BUFFER_SIZE]; // Use a contigous data structure, i.e. an array, to ensure cache locality.
Point output_buffer[BUFFER_SIZE]; // Note: one could perform the conversion in-place, hence halfing the memory usage, but it should be more robust to use separate memory for the buffers since one may decide to change the output data struct and then forget to change also the underlying memory.

int atritec(char* filename)
{
    clock_t t0 = clock();
    // It is assumed that in general the file size can be huge, multiple terabytes. On some platforms and operating system that support memory swapping loading the data fits in the process' virtual memory space. Since, I don't know what platform and OS we are targeting I decide to use a buffer to write from an input file to an output file in batches.
    FILE* fp_in = fopen(filename, "rb");
    if (fp_in == 0)
    {
        printf("Failed to open file `%s`.\n", filename);
        return 1;
    }
    FILE* fp_out = fopen("output.bin","wb");
    if (fp_out == 0)
    {
        printf("Failed to open file `output.bin`.\n");
        fclose(fp_in);
        return 1;
    }

    int done = 0;
    for (;done == 0;)
    {
        /* Populate input buffer */
        // Note: There is no mention of endianess in the given data schema. Hence, it is assumed that the endianss of the platform that produces the data is the same as the endianess of the platform that will run this code.
        // It is assumed that the struct has been written field by field for space efficiency and portability.
        // For example, according to the source code the size of AtrisenseRecord is 18 bytes, assuming sizeof(float) = 4, but after compilation it is 20 due to 2 bytes of padding at the end to satisfy data structure alignment. Furthermore, if on some exotic platform sizeof(float) = 8, then there would be 10 bytes of padding.
        // Both the reading and writing could be significantly simplified if either one uses compiler commands to
        // pack the structs or one had used a uint32 for the field `intensity`. The former is implementation dependent
        // and the latter I lack control over. Hence, I choose to read and write field by field. One could have
        // replaced all read calls by just `fread(input_buffer, sizeof(AtrisenseRecord), BUFFER_SIZE, fp_in)`
        // if there was no padding.
        int n_data = fread(input_buffer, atrisense_record_sz, BUFFER_SIZE, fp_in);
        if (n_data < BUFFER_SIZE)
        {
            if (feof(fp_in))
            {
                done = 1;
            }
            if (ferror(fp_in))
            {
                printf("Failed to read field 'scan_number' of datum %d\n", n_data);
                return 1;
            }
        }
        /* Compute 3D points from the spherical points. */
        for (int i = 0; i < n_data; i++)
        {
            AtrisenseRecord* d = (AtrisenseRecord*)&input_buffer[atrisense_record_sz*i];
            float deg2rad = 3.14159265 / 180.0; // Assume that this many digits of PI is sufficient
            output_buffer[i].scan_number = d->scan_number;
            output_buffer[i].x = d->distance_m*cos(deg2rad*d->y_angle_deg)*cos(deg2rad*d->x_angle_deg);
            output_buffer[i].y = d->distance_m*cos(deg2rad*d->y_angle_deg)*sin(deg2rad*d->x_angle_deg);
            output_buffer[i].z = d->distance_m*sin(deg2rad*d->y_angle_deg);
            output_buffer[i].intensity = d->intensity;
        }

        /* Write converted data to disk. */
        for (int i = 0; i < n_data; i++)
        {
            size_t n_written = fwrite(&output_buffer[i], point_sz, 1, fp_out);
            if (n_written == 0)
            {
                printf("Failed to write record %d.\n", i);
                return 1;
            }
        }
    }

    if (fclose(fp_in)== EOF)
    {
        printf("Failed to close file '%s'\n", filename);
        return 1;
    }
    if (fclose(fp_out)== EOF)
    {
        printf("Failed to close file 'output.bin'\n");
        return 1;
    }
    clock_t t_final = clock();
    printf("time = %f\n", (t_final - t0)/(double)CLOCKS_PER_SEC);
    return 0;
}
