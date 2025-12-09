#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <cstring>

typedef struct {
    uint32_t scan_number;
    float x_angle_deg;
    float y_angle_deg;
    float distance_m;
    uint16_t intensity;
} AtrisenseRecord;

typedef struct {
    uint32_t scan_number;
    float x;
    float y;
    float z;
    uint16_t intensity;
} Point;

int atritec(char* filename)
{
    FILE* fp = fopen(filename, "rb");
    if (fp == 0)
    {
        printf("Failed to open file %s.\n", filename);
        return 1;
    }

    /* Parse data. */
    // Use a contigous data structure, i.e. an array, to ensure cache locality.
    int max_n_data = 64; // Should be tuned to expected `max_n_data` of application. Then one can skip the memory reallocations when one "runs out of memory".
    AtrisenseRecord* data = (AtrisenseRecord*)malloc(sizeof(AtrisenseRecord)*max_n_data); // Assume that using stdlib malloc is sufficient for our memory handling
    if (data == 0)
    {
        printf("Failed to allocate memory for input data\n");
        fclose(fp);
        return 1;
    }
    int n_data = 0;
    int parsing_succeeded = 1;
    size_t n_read;
    for (;;)
    {
        int c = fgetc(fp);
        if ( c == EOF)
        {
            break;
        }
        else
        {
            ungetc(c, fp);
        }
        // Read field by field to ensure portability. A struct can contain padding. On some platforms direct writing of a struct to disk can include padding between fields. Although, I have never encountered this. Still, do it for robustness.
        n_read = fread(&data[n_data].scan_number, sizeof(data[n_data].scan_number),1, fp);
        if (n_read == 0)
        {
            printf("Failed to read field 'scan_number' of datum %d\n", n_data);
            parsing_succeeded = 0;
            break;
        }
        n_read = fread(&data[n_data].x_angle_deg, sizeof(data[n_data].x_angle_deg),1, fp);
        if (n_read == 0)
        {
            printf("Failed to read field 'x_angle_deg' of datum %d\n", n_data);
            parsing_succeeded = 0;
            break;
        }
        n_read= fread(&data[n_data].y_angle_deg, sizeof(data[n_data].y_angle_deg),1, fp);
        if (n_read == 0)
        {
            printf("Failed to read field 'y_angle_deg' of datum %d\n", n_data);
            parsing_succeeded = 0;
            break;
        }
        n_read= fread(&data[n_data].distance_m, sizeof(data[n_data].distance_m),1, fp);
        if (n_read == 0)
        {
            printf("Failed to read field 'distance_m' of datum %d\n", n_data);
            parsing_succeeded = 0;
            break;
        }
        n_read= fread(&data[n_data].intensity, sizeof(data[n_data].intensity),1, fp);
        if (n_read == 0)
        {
            printf("Failed to read field 'intensity' of datum %d\n", n_data);
            parsing_succeeded = 0;
            break;
        }
        n_data++;
        if (n_data == max_n_data)
        {
            max_n_data *= 2; // Assume that exponential growth is OK for our application. Especially, a user will not provide a file that results in unbounded reading.
            AtrisenseRecord* tmp = (AtrisenseRecord*)malloc(sizeof(AtrisenseRecord)*max_n_data);
            if (tmp == 0)
            {
                printf("Failed to reallocate memory\n");
                parsing_succeeded = 0;
                break;
            }
            memcpy(tmp, data, sizeof(AtrisenseRecord)*n_data);
            free(data);
            data = tmp;
        }
    }
    if (fclose(fp) == EOF)
    {
        printf("Failed to close file\n");
        parsing_succeeded = 0;
    }

    if (parsing_succeeded == 0)
    {
        printf("Parsing failed\n");
        free(data);
        return 1;
    }

    /* Compute 3D points from the spherical points. */
    Point* points = (Point*)malloc(sizeof(Point)*n_data);
    if (points == 0)
    {
        printf("Failed to allocate memory for points\n");
        free(data);
        return 1;
    }
    for (int i = 0; i < n_data; i++)
    {
        AtrisenseRecord d = data[i];
        float deg2rad = 3.14159265 / 180.0; // Assume that this many digits of PI is sufficient
        points[i].scan_number = d.scan_number;
        points[i].x = d.distance_m*sin(deg2rad*d.y_angle_deg)*cos(deg2rad*d.x_angle_deg);
        points[i].y = d.distance_m*cos(deg2rad*d.y_angle_deg);
        points[i].z = d.distance_m*sin(deg2rad*d.y_angle_deg)*sin(deg2rad*d.x_angle_deg);

        points[i].intensity = d.intensity;
    }
    free(data);

    /* Write converted data to disk. */
    fp = fopen("output.bin","wb");
    if (fp == 0)
    {
        printf("Failed to open 'output.bin'\n");
        free(points);
        return 1;
    }
    size_t n_written;
    for (int i = 0; i < n_data; i++)
    {
        n_written = fwrite(&points[i].scan_number, sizeof(points[i].scan_number),1,fp);
        if (n_written == 0)
        {
            printf("Failed to write field 'scan_number' of datum %d\n", i);
            break;
        }

        n_written = fwrite(&points[i].x, sizeof(points[i].x),1,fp);
        if (n_written == 0)
        {
            printf("Failed to write field 'x' of datum %d\n", i);
            break;
        }

        n_written = fwrite(&points[i].y, sizeof(points[i].y),1,fp);
        if (n_written == 0)
        {
            printf("Failed to write field 'y' of datum %d\n", i);
            break;
        }

        n_written = fwrite(&points[i].z, sizeof(points[i].z),1,fp);
        if (n_written == 0)
        {
            printf("Failed to write field 'z' of datum %d\n", i);
            break;
        }
        n_written = fwrite(&points[i].intensity, sizeof(points[i].intensity),1,fp);
        if (n_written == 0)
        {
            printf("Failed to write field 'intensity' of datum %d\n", i);
            break;
        }
    }
    free(points);

    if (fclose(fp)== EOF)
    {
        printf("Failed to close file 'output.bin'\n");
        return 1;
    }

    return 0;
}