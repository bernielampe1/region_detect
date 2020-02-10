#ifndef _PIXEL_H_
#define _PIXEL_H_

template< typename T >  class Pixel
{
  private:
    signed r, c;
    T val;

  public:
    Pixel(): r(0), c(0), val(0) {}
    Pixel(const signed _r, const signed _c, const T _v): r(_r), c(_c), val(_v){}

    // accessors
    signed getRow() const { return(r); }
    signed getCol() const { return(c); }
    T value() const { return(val); }

    bool operator<(const Pixel &rhs) const
    {
      if (val < rhs.val) return(true);
      else if (val > rhs.val) return(false);

      if (r < rhs.r) return(true);
      else if (r > rhs.r) return(false);

      if (c < rhs.c) return(true);
      else if (c > rhs.c) return(false);

      return(false);
    }

    bool operator>(const Pixel &rhs) const
    {
      if (val > rhs.val) return(true);
      else if (val < rhs.val) return(false);

      if (r > rhs.r) return(true);
      else if (r < rhs.r) return(false);

      if (c > rhs.c) return(true);
      else if (c < rhs.c) return(false);

      return(false);
    }

    bool operator==(const Pixel &rhs) const
    {
      return (val == rhs.val && r == rhs.r && c == rhs.c);
    }

    bool operator!=(const Pixel &rhs) const
    {
      return (!(this == rhs));
    }
};

#endif // _PIXEL_H_
