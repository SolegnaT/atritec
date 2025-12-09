#include<cstdio>
#include"atritec.cpp"
#include<cstring>

// Note: Run test on source code instead of on a binary because this simplfies debugging.
// Note: Instead of implementing a test framework one could use GoogleTest.

int test1()
{
    int result;
    FILE* fp;

    /* Generate input*/
    fp = fopen("test1.bin","wb");
    AtrisenseRecord data[3];

    // x-axis
    data[0].scan_number = 0;
    data[0].x_angle_deg = 0.0;
    data[0].y_angle_deg = 90.0;
    data[0].distance_m = 1.0;
    data[0].intensity = 1;

    // y-axis
    data[1].scan_number = 1;
    data[1].x_angle_deg = 0.0;
    data[1].y_angle_deg = 0.0;
    data[1].distance_m = 1.0;
    data[1].intensity = 2;

    // z-axis
    data[2].scan_number = 2;
    data[2].x_angle_deg = 90.0;
    data[2].y_angle_deg = 90.0;
    data[2].distance_m = 1.0;
    data[2].intensity = 2;
    result = 1;
    for (int i = 0; i < 3; i++)
    {
        result *= fwrite(&data[i].scan_number, sizeof(data[i].scan_number), 1, fp);
        result *= fwrite(&data[i].x_angle_deg, sizeof(data[i].x_angle_deg), 1, fp);
        result *= fwrite(&data[i].y_angle_deg, sizeof(data[i].y_angle_deg), 1, fp);
        result *= fwrite(&data[i].distance_m, sizeof(data[i].distance_m), 1, fp);
        result *= fwrite(&data[i].intensity, sizeof(data[i].intensity), 1, fp);
    }
    fclose(fp);
    if (result == 0)
    {
        printf("Failed to write 'test1.bin'\n");
        return 1;
    }


    /* Execute 'atritec' */
    result = atritec("test1.bin");
    if (result == 1)
    {
        printf("Failed to run 'atritec'\n");
        return 1;
    }

    fp = fopen("output.bin", "rb");
    Point points[3];
    result = 1;
    for (int i = 0; i < 3; i++)
    {
        result *= fread(&points[i].scan_number, sizeof(points[i].scan_number), 1, fp);
        result *= fread(&points[i].x, sizeof(points[i].x), 1, fp);
        result *= fread(&points[i].y, sizeof(points[i].y), 1, fp);
        result *= fread(&points[i].z, sizeof(points[i].z), 1, fp);
        result *= fread(&points[i].intensity, sizeof(points[i].intensity), 1, fp);
    }
    fclose(fp);
    if (result == 0)
    {
        printf("Failed to read 'output.bin'.\n");
        return 1;
    }

    Point expected[3];
    expected[0].scan_number = data[0].scan_number;
    expected[0].x = 1.0;
    expected[0].y = 0.0;
    expected[0].z = 0.0;
    expected[0].intensity = data[0].intensity;

    expected[1].scan_number = data[1].scan_number;
    expected[1].x = 0.0;
    expected[1].y = 1.0;
    expected[1].z = 0.0;
    expected[1].intensity = data[1].intensity;

    expected[2].scan_number = data[2].scan_number;
    expected[2].x = 0.0;
    expected[2].y = 0.0;
    expected[2].z = 1.0;
    expected[2].intensity = data[2].intensity;

    float tolerance = 1e-6;
    for (int i = 0; i<  3; i++)
    {
        result = expected[i].scan_number == points[i].scan_number;
        if (result == 0)
            return 1;
        result = fabsf(expected[i].x - points[i].x) < tolerance;
        if (result == 0)
            return 1;
        result = fabsf(expected[i].y - points[i].y) < tolerance;
        if (result == 0)
            return 1;
        result = fabsf(expected[i].z - points[i].z) < tolerance;
        if (result == 0)
            return 1;
        result = expected[i].intensity == points[i].intensity;
        if (result == 0)
            return 1;
    }
    return 0;
}

int test2()
{
    int result;

    FILE* fp = fopen("test_input.bin","wb");
    char s[16] = "some junk";
    result = fwrite(s, sizeof(s)*16, 1, fp);
    fclose(fp);
    if (result == 0)
    {
        printf("Failed to write.\n");
    }

    result = atritec("test_input.bin");
    if (result == 1)
    {
        return 0;
    }
    else
    {
        return 1;
    }

}

int main(int nargs, char** vargs)
{
    int result[64];
    result[0] = test1();
    result[1] = test2();
    int n_tests = 2;

    int n_fail = 0;
    for (int i = 0; i < n_tests; i++)
    {
        n_fail += result[i];
        if (result[i] == 1)
        {
            printf("Failed test %d\n",i+1);
        }
    }
    printf("Failed %d tests out of %d tests.\n",n_fail, n_tests);
}