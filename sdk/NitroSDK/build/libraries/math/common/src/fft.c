/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MATH -
  File:     fft.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: fft.c,v $
  Revision 1.3  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.2  05/17/2005 12:55:26  seiki_masashi
  Support for warning.

  Revision 1.1  05/13/2005 09:23:09  seiki_masashi
  Added FFT function

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nitro/fx/fx_trig.h>

/*---------------------------------------------------------------------------*
  Name:         MATH_MakeFFTSinTable

  Description:  Creates sin table used in fast Fourier transform.

  Arguments:    sinTable - Pointer to location storing 2^nShift * 3/4  sin table
                nShift - log2 of the number of data

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATH_MakeFFTSinTable(fx16 *sinTable, u32 nShift)
{
    u32     i;
    u32     n = 1U << nShift;
    u32     nq3 = (n >> 1) + (n >> 2);
    s32     w;
    u32     dw;

    SDK_ASSERT(nShift < 32);

    dw = 65536U >> nShift;
    for (i = 0, w = 0; i < nq3; i++, w += dw)
    {
        sinTable[i] = FX_SinIdx(w);
    }
}

/*---------------------------------------------------------------------------*
  Name:         MATHi_FFT

  Description:  Internal function that performs fast Fourier transform.

  Arguments:    data - Data to transform, with even-numbered data being real and odd-numbered data being imaginary.
                       The transformation result will be returned overwritten.
                nShift - log2 of the number of data
                sinTable - Sin value table based on a circle divided into equal sections; The number of sections is the number of data

  Returns:      None.
 *---------------------------------------------------------------------------*/
#define SWAP_FX32(a, b) {fx32 t; t = (a); (a) = (b); (b) = t;}
void MATHi_FFT(fx32 *data, u32 nShift, const fx16 *sinTable)
{
    u32     i, j;
    u32     n = 1U << nShift;
    u32     n2 = n << 1;
    u32     nq = n >> 2;

    // Bit inversion
    {
        u32     rev = 0;
        u32     shift = 32 - nShift - 1;        // -1 is used to double j
        for (i = 0; i < n2; i += 2)
        {
            j = rev >> shift;          // Logical shift
            if (i < j)
            {
                // i and j have already been doubled
                SWAP_FX32(data[i], data[j]);
                SWAP_FX32(data[i + 1], data[j + 1]);
            }
            // Treat rev as a 32-bit inverted integer and increment
            {
                u32     s;
                s = MATH_CLZ(~rev);
                rev ^= (((s32)(0x80000000U)) >> s);     // Arithmetic shift
            }
        }
    }

    // Butterfly
    // To Do: Adjust the loop order so that memory can be accessed continuously.
    {
        u32     div, dt;

        dt = n;
        // Build up FFT from small divisions
        // Note that div and k are doubled for future use
        for (div = 2; div < n2; div <<= 1)
        {
            u32     k;
            u32     di = div * 2;
            u32     t;
            dt >>= 1;

            // Perform calculation for 0th term in each division
            for (i = 0; i < n2; i += di)
            {
                fx32    xr, xi, yr, yi;
                u32     j = i + div;
                // i and j have already been doubled

                // 0th term can be calculated with simple addition and subtraction.
                // (wr, wi) == (1, 0)
                xr = data[i];
                xi = data[i + 1];
                yr = data[j];
                yi = data[j + 1];
                data[i] = xr + yr;
                data[i + 1] = xi + yi;
                data[j] = xr - yr;
                data[j + 1] = xi - yi;
            }

            t = dt;

            // Calculate Kth terms that use the same twiddle factor at once.
            for (k = 2; k < div; k += 2)
            {
                if (k != div / 2)
                {
                    fx32    wr, wi, w1, w2;
                    wr = sinTable[t + nq];      // cos(-x) == cos(x) == sin(x+(pi/2))
                    wi = -sinTable[t];
//                    if (inverse)
//                    {
//                        wi = -wi; // sin(-x) = -sin(x)
//                    }
                    t += dt;

                    w1 = wr + wi;
                    w2 = wr - wi;

                    // Perform calculation for Kth term in each division
                    for (i = k; i < n2; i += di)
                    {
                        fx32    xr, xi, yr, yi;
                        u32     j = i + div;
                        // i and j have already been doubled

                        // Rotate
                        xr = data[j];
                        xi = data[j + 1];
                        {
                            // Same as the following process
                            // yr = FX_Mul(wr, xr) - FX_Mul(wi, xi);
                            // yi = FX_Mul(wr, xi) + FX_Mul(wi, xr);
                            fx32    t = FX_Mul(wr, xr + xi);
                            yr = t - FX_Mul(w1, xi);
                            yi = t - FX_Mul(w2, xr);
                        }

                        xr = data[i];
                        xi = data[i + 1];
                        data[i] = xr + yr;
                        data[i + 1] = xi + yi;
                        data[j] = xr - yr;
                        data[j + 1] = xi - yi;
                    }
                }
                else
                {
                    t += dt;
                    // Perform calculation for Kth term in each division
                    for (i = k; i < n2; i += di)
                    {
                        fx32    xr, xi, yr, yi;
                        u32     j = i + div;
                        // i and j have already been doubled

                        // div/2-th term can be calculated with simple addition and subtraction.
                        // (wr, wi) == (0, 1)
                        xr = data[i];
                        xi = data[i + 1];
                        yr = data[j];
                        yi = data[j + 1];
//                        if (inverse)
//                        {
//                            yr = -yr; yi = -yi;
//                        }
                        data[i] = xr + yi;
                        data[i + 1] = xi - yr;
                        data[j] = xr - yi;
                        data[j + 1] = xi + yr;
                    }
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         MATHi_IFFT

  Description:  Internal function that reverses the fast Fourier transform.

  Arguments:    data - Data to transform, with even-numbered data being real and odd-numbered data being imaginary.
                       The transformation result will be returned overwritten.
                nShift - log2 of the number of data
                sinTable - Sin value table based on a circle divided into equal sections; The number of sections is the number of data

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATHi_IFFT(fx32 *data, u32 nShift, const fx16 *sinTable)
{
    u32     i, j;
    u32     n = 1U << nShift;
    u32     n2 = n << 1;
    u32     nq = n >> 2;

    // Bit inversion
    {
        u32     rev = 0;
        u32     shift = 32 - nShift - 1;        // -1 is used to double j
        for (i = 0; i < n2; i += 2)
        {
            j = rev >> shift;          // Logical shift
            if (i < j)
            {
                // i and j have already been doubled
                SWAP_FX32(data[i], data[j]);
                SWAP_FX32(data[i + 1], data[j + 1]);
            }
            // Treat rev as a 32-bit inverted integer and increment
            {
                u32     s;
                s = MATH_CLZ(~rev);
                rev ^= (((s32)(0x80000000U)) >> s);     // Arithmetic shift
            }
        }
    }

    // Butterfly
    // To Do: Adjust the loop order so that memory can be accessed continuously.
    {
        u32     div, dt;

        dt = n;
        // Build up FFT from small divisions
        // Note that div and k are doubled for future use
        for (div = 2; div < n2; div <<= 1)
        {
            u32     k;
            u32     di = div * 2;
            u32     t;
            dt >>= 1;

            // Perform calculation for 0th term in each division
            for (i = 0; i < n2; i += di)
            {
                fx32    xr, xi, yr, yi;
                u32     j = i + div;
                // i and j have already been doubled

                // 0th term can be calculated with simple addition and subtraction.
                // (wr, wi) == (1, 0)
                xr = data[i];
                xi = data[i + 1];
                yr = data[j];
                yi = data[j + 1];
                data[i] = xr + yr;
                data[i + 1] = xi + yi;
                data[j] = xr - yr;
                data[j + 1] = xi - yi;
            }

            t = dt;

            // Calculate Kth terms that use the same twiddle factor at once.
            for (k = 2; k < div; k += 2)
            {
                if (k != div / 2)
                {
                    fx32    wr, wi, w1, w2;
                    wr = sinTable[t + nq];      // cos(-x) == cos(x) == sin(x+(pi/2))
                    wi = sinTable[t];
                    t += dt;

                    w1 = wr + wi;
                    w2 = wr - wi;

                    // Perform calculation for Kth term in each division
                    for (i = k; i < n2; i += di)
                    {
                        fx32    xr, xi, yr, yi;
                        u32     j = i + div;
                        // i and j have already been doubled

                        // Rotate
                        xr = data[j];
                        xi = data[j + 1];
                        {
                            // Same as the following process
                            // yr = FX_Mul(wr, xr) - FX_Mul(wi, xi);
                            // yi = FX_Mul(wr, xi) + FX_Mul(wi, xr);
                            fx32    t = FX_Mul(wr, xr + xi);
                            yr = t - FX_Mul(w1, xi);
                            yi = t - FX_Mul(w2, xr);
                        }

                        xr = data[i];
                        xi = data[i + 1];
                        data[i] = xr + yr;
                        data[i + 1] = xi + yi;
                        data[j] = xr - yr;
                        data[j + 1] = xi - yi;
                    }
                }
                else
                {
                    t += dt;
                    // Perform calculation for Kth term in each division
                    for (i = k; i < n2; i += di)
                    {
                        fx32    xr, xi, yr, yi;
                        u32     j = i + div;
                        // i and j have already been doubled

                        // div/2-th term can be calculated with simple addition and subtraction.
                        // (wr, wi) == (0, 1)
                        xr = data[i];
                        xi = data[i + 1];
                        yr = data[j];
                        yi = data[j + 1];
                        data[i] = xr - yi;
                        data[i + 1] = xi + yr;
                        data[j] = xr + yi;
                        data[j + 1] = xi - yr;
                    }
                }
            }
        }
    }
}

#if 0
void MATHi_FFT_alter(fx32 *data, u32 nShift, const fx16 *sinTable, BOOL inverse)
{
    u32     i, j;
    u32     n = 1 << nShift;
    u32     n2 = n << 1;
    u32     nq = n >> 2;

    // Bit inversion
    {
        u32     rev = 0;
        u32     shift = 32 - nShift - 1;        // -1 is used to double j
        for (i = 0; i < n2; i += 2)
        {
            j = rev >> shift;          // Logical shift
            if (i < j)
            {
                // i and j have already been doubled
                SWAP_FX32(data[i], data[j]);
                SWAP_FX32(data[i + 1], data[j + 1]);
            }
            // Treat rev as a 32-bit inverted integer and increment
            {
                u32     s;
                s = MATH_CLZ(~rev);
                rev ^= (((s32)(0x80000000U)) >> s);     // Arithmetic shift
            }
        }
    }

    // Butterfly
    {
        u32     div, dt;

        dt = n;
        // Build up FFT from small divisions
        // Note that div and k are doubled for future use
        for (div = 2; div < n2; div <<= 1)
        {
            u32     k;
            u32     di = div * 2;
            u32     t;
            dt >>= 1;

            for (i = 0; i < n2; i += di)
            {
                {
                    fx32    xr, xi, yr, yi;
                    u32     j = i + div;
                    // i and j have already been doubled

                    // 0th term can be calculated with simple addition and subtraction.
                    xr = data[i];
                    xi = data[i + 1];
                    yr = data[j];
                    yi = data[j + 1];
                    data[i] = xr + yr;
                    data[i + 1] = xi + yi;
                    data[j] = xr - yr;
                    data[j + 1] = xi - yi;
                }

                t = dt;

                for (k = 2; k < div; k += 2)
                {
                    fx32    wr, wi;
                    wr = sinTable[t + nq];      // cos(-x) == cos(x) == sin(x+(pi/2))
                    wi = sinTable[t];
                    if (!inverse)
                    {
                        wi = -wi;      // sin(-x) = -sin(x)
                    }
                    t += dt;

                    {
                        fx32    xr, xi, yr, yi;
                        u32     ik = i + k;
                        u32     j = ik + div;
                        // ik and j have already been doubled

                        // Rotate
                        xr = data[j];
                        xi = data[j + 1];
                        {
                            // Same as the following process
                            // yr = FX_Mul(wr, xr) - FX_Mul(wi, xi);
                            // yi = FX_Mul(wr, xi) + FX_Mul(wi, xr);
                            fx32    t = FX_Mul(wr, xr + xi);
                            yr = t - FX_Mul(wr + wi, xi);
                            yi = t - FX_Mul(wr - wi, xr);
                        }

                        xr = data[ik];
                        xi = data[ik + 1];
                        data[ik] = xr + yr;
                        data[ik + 1] = xi + yi;
                        data[j] = xr - yr;
                        data[j + 1] = xi - yi;
                    }
                }
            }
        }
    }
}
#endif

/*---------------------------------------------------------------------------*
  Name:         MATH_FFT

  Description:  Performs fast Fourier transform.

  Arguments:    data - Data to transform, with even-numbered data being real and odd-numbered data being imaginary.
                       The transformation result will be returned overwritten.
                nShift - log2 of the number of data
                sinTable - Sin value table based on a circle divided into equal sections; The number of sections is the number of data

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATH_FFT(fx32 *data, u32 nShift, const fx16 *sinTable)
{
    u32     i;
    u32     n = 1U << nShift;
    u32     n2 = n * 2;

    MATHi_FFT(data, nShift, sinTable);

    // Divide the result by n
    for (i = 0; i < n2; i++)
    {
        data[i] >>= nShift;
    }
}

/*---------------------------------------------------------------------------*
  Name:         MATH_IFFT

  Description:  Performs the inverse transformation of fast Fourier transform.

  Arguments:    data - Data to transform, with even-numbered data being real and odd-numbered data being imaginary.
                       The transformation result will be returned overwritten.
                nShift - log2 of the number of data
                sinTable - Sin value table based on a circle divided into equal sections; The number of sections is the number of data

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATH_IFFT(fx32 *data, u32 nShift, const fx16 *sinTable)
{
    MATHi_IFFT(data, nShift, sinTable);
}

/*---------------------------------------------------------------------------*
  Name:         MATH_FFTReal

  Description:  Performs fast Fourier transform.

  Arguments:    data - Data that includes only the real parts.
                       The transformation result will be returned overwritten.
                nShift - log2 of the number of data
                sinTable - Sin value table based on a circle divided into equal sections; The number of sections is the number of data

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATH_FFTReal(fx32 *data, u32 nShift, const fx16 *sinTable, const fx16 *sinTable2)
{
    u32     i, j, k;
    u32     n = 1U << nShift;
    u32     nq = n >> 2;

    MATHi_FFT(data, nShift - 1, sinTable2);

    for (k = 1; k < nq; k++)
    {
        fx32    xr, xi, yr, yi, zr, zi;
        fx32    wr, wi;

        i = k * 2;
        j = n - i;                     // == (n/2-k)*2

        wr = sinTable[k + nq];         // sin(x+(pi/2)) == cos(x)
        wi = sinTable[k];

        {
            fx32    d1r, d1i, d2r, d2i;
            d1r = data[i];
            d1i = data[i + 1];
            d2r = data[j];
            d2i = data[j + 1];
            yr = (d1r + d2r) >> 1;
            yi = (d1i - d2i) >> 1;
            zr = (d1i + d2i) >> 1;
            zi = -(d1r - d2r) >> 1;
        }

        // xr = FX_Mul(wr, zr) - FX_Mul(wi, zi);
        // xi = FX_Mul(wr, zi) + FX_Mul(wi, zr);
        {
            fx32    t = FX_Mul(wr, zr + zi);
            xr = t - FX_Mul(wr + wi, zi);
            xi = t - FX_Mul(wr - wi, zr);
        }

        data[i] = yr + xr;
        data[i + 1] = yi + xi;
        data[j] = yr - xr;
        data[j + 1] = -yi + xi;
    }
    {
        fx32    xr, xi;
        xr = data[0];
        xi = data[1];
        data[0] = xr + xi;
        data[1] = xr - xi;
    }

    for (i = 0; i < n; i++)
    {
        data[i] >>= nShift;
    }

}

/*---------------------------------------------------------------------------*
  Name:         MATH_IFFTReal

  Description:  Performs the inverse transformation of fast Fourier transform.

  Arguments:    data - Data that includes only the real parts.
                       The transformation result will be returned overwritten.
                nShift - log2 of the number of data
                sinTable - Sin value table based on a circle divided into equal sections; The number of sections is the number of data

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATH_IFFTReal(fx32 *data, u32 nShift, const fx16 *sinTable, const fx16 *sinTable2)
{
    u32     i, j, k;
    u32     n = 1U << nShift;
    u32     nq = n >> 2;

    for (k = 1; k < nq; k++)
    {
        fx32    yr, yi, zr, zi, xr, xi;
        fx32    wr, wi;

        i = k * 2;
        j = n - i;                     // == (n/2-k)*2

        wr = sinTable[k + nq];         // cos(-x) == sin(x+(pi/2))
        wi = -sinTable[k];             // sin(-x) == -sin(x)

        {
            fx32    d1r, d1i, d2r, d2i;
            d1r = data[i];
            d1i = data[i + 1];
            d2r = data[j];
            d2i = data[j + 1];
            yr = (d1r + d2r);
            yi = (d1i - d2i);
            zr = -(d1i + d2i);
            zi = (d1r - d2r);
        }

        // xr = FX_Mul(wr, zr) - FX_Mul(wi, zi);
        // xi = FX_Mul(wr, zi) + FX_Mul(wi, zr);
        {
            fx32    t = FX_Mul(wr, zr + zi);
            xr = t - FX_Mul(wr + wi, zi);
            xi = t - FX_Mul(wr - wi, zr);
        }

        data[i] = yr + xr;
        data[i + 1] = yi + xi;
        data[j] = yr - xr;
        data[j + 1] = -yi + xi;
    }
    {
        fx32    xr, xi;
        xr = data[0];
        xi = data[1];
        data[0] = (xr + xi);
        data[1] = (xr - xi);
        data[n / 2] <<= 1;
        data[n / 2 + 1] <<= 1;
    }

    MATHi_IFFT(data, nShift - 1, sinTable2);
}
