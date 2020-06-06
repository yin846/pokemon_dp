
#include <nnsys/g2d/g2d_PaletteTable.h>

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitializePaletteTable

  Description:  Initializes the palette conversion table
                
  Arguments:    pPlttTbl:      [OUT] Palette instance
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dInitializePaletteTable( NNSG2dPaletteSwapTable* pPlttTbl )
{
    u16     i;
    NNS_G2D_NULL_ASSERT( pPlttTbl );
    
    
    // Set to no switching
    for( i = 0; i < NNS_G2D_NUM_COLOR_PALETTE; i++ )
    {
        pPlttTbl->paletteIndex[i] = i;
    }    
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitializePaletteTable

  Description:  Configures the palette conversion table
                
  Arguments:    pPlttTbl:       [OUT] Palette instance
                beforeIdx:      [IN] The palette number before conversion
                afterIdx:       [IN] The palette number after conversion
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetPaletteTableValue
( 
    NNSG2dPaletteSwapTable* pPlttTbl, 
    u16                     beforeIdx, 
    u16                     afterIdx 
)
{
    NNS_G2D_NULL_ASSERT( pPlttTbl );
    NNS_G2D_MINMAX_ASSERT( beforeIdx, 0, NNS_G2D_NUM_COLOR_PALETTE );
    NNS_G2D_MINMAX_ASSERT( afterIdx, 0, NNS_G2D_NUM_COLOR_PALETTE );
    
    pPlttTbl->paletteIndex[beforeIdx] = afterIdx;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitializePaletteTable

  Description:  Gets the settings of the palette conversion table
                
  Arguments:    pPlttTbl:      [IN]  Palette instance
                beforeIdx:     [IN] The palette number before conversion
                
  Returns:      The palette number after conversion
  
 *---------------------------------------------------------------------------*/
u16 NNS_G2dGetPaletteTableValue( const NNSG2dPaletteSwapTable* pPlttTbl, u16 beforeIdx )
{
    NNS_G2D_NULL_ASSERT( pPlttTbl );
    NNS_G2D_MINMAX_ASSERT( beforeIdx, 0, NNS_G2D_NUM_COLOR_PALETTE );
    
    {
        const u16       afterIdx = pPlttTbl->paletteIndex[beforeIdx];
        NNS_G2D_MINMAX_ASSERT( afterIdx, 0, NNS_G2D_NUM_COLOR_PALETTE );
        return afterIdx;
    }
}


