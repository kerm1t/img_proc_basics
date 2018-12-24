#pragma once

/*
  12/24/2018 speed up drawing with GDI ... hmm needs MFC, so better switch to OpenGL again
*/

#include "stdafx.h"
#include "afxwin.h" // <-- neeeds installation of MFC --> nope!

// s. https://www.codeproject.com/Articles/33/Flicker-Free-Drawing-In-MFC  ... this is the app which wants to install MFC
// s. https://social.msdn.microsoft.com/Forums/en-US/65f2f6f9-ff85-450a-a80e-65bcc412b2a6/performance-issue-when-drawing-on-mfc?forum=vcgeneral
// https://stackoverflow.com/questions/10840464/gdi-acceleration-in-windows-7-drawing-to-memory-bitmap/10841445#10841445

#define NULL nullptr

class CGdiDblBuf
{
  Graphics *m_gr;
  CDC      *m_pMemDC; // include afxwin.h (s. above) ... https://docs.microsoft.com/en-us/cpp/mfc/reference/cdc-class?view=vs-2017 s. "Requirements"
  CBitmap  *m_pbmpMemBitmap;

  CGdiDblBuf()
  {
    m_pMemDC = NULL;
    m_gr = NULL;
    m_pbmpMemBitmap = NULL;
  }

  ~CGdiDblBuf()
  {
    // Clean up all the GDI and GDI+ objects we've used
    if (m_pMemDC)
    {
      delete m_pMemDC; m_pMemDC = NULL;
    }
    if (m_pbmpMemBitmap)
    {
      delete m_pbmpMemBitmap; m_pbmpMemBitmap = NULL;
    }
    if (m_gr)
    {
      delete m_gr; m_gr = NULL;
    }
  }

  void OnPaint()
  {
    pDC->BitBlt(rcUpdate.left, rcUpdate.top, rcUpdate.Width(), rcUpdate.Height(),
      m_pMemDC, rcUpdate.left, rcUpdate.top, SRCCOPY);
  }

  void vCreateScreenBuffer(const CSize szPanel, CDC *pDesktopDC)
    // In : 
    //      szPanel = The size that we want the double buffer bitmap to be
    // Out : None
  {
    // Delete anything we're already using first
    if (m_pMemDC)
    {
      delete m_gr;
      m_gr = NULL;
      delete m_pMemDC;
      m_pMemDC = NULL;
      delete m_pbmpMemBitmap;
      m_pbmpMemBitmap = NULL;
    }
    // Make a compatible DC
    m_pMemDC = new CDC;
    m_pMemDC->CreateCompatibleDC(pDesktopDC);
    // Create a new bitmap
    m_pbmpMemBitmap = new CBitmap;
    // Create the new bitmap
    m_pbmpMemBitmap->CreateCompatibleBitmap(pDesktopDC, szPanel.cx, szPanel.cy);
    m_pbmpMemBitmap->SetBitmapDimension(szPanel.cx, szPanel.cy);
    // Select the new bitmap into the memory DC
    m_pMemDC->SelectObject(m_pbmpMemBitmap);
    // Then create a GDI+ Graphics object
    m_gr = Graphics::FromHDC(m_pMemDC->m_hDC);
    // And update the bitmap
    rcUpdateBitmap(rcNewSize, true);
  }

  CRect rcUpdateBitmap(const CRect &rcInvalid, const bool bInvalidate, const bool bDrawBackground /*=true*/)
    // Redraws an area of the double buffered bitmap
    // In : 
    //      rcInvalid - The rect to redraw
    //      bInvalidate - Whether to refresh to the screen when we're done
    //      bDrawBackground - Whether to draw the background first (can give speed benefits if we don't need to)
    // Out : None
  {
    // The memory bitmap is actually updated here

    // Then make the screen update
    if (bInvalidate)
    {
      InvalidateRect(rcInvalid);
    }
  }
}