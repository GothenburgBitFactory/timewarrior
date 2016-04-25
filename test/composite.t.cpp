////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2015 - 2016, Paul Beckingham, Federico Hernandez.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// http://www.opensource.org/licenses/mit-license.php
//
////////////////////////////////////////////////////////////////////////////////

#include <cmake.h>
#include <Composite.h>
#include <test.h>

////////////////////////////////////////////////////////////////////////////////
int main (int, char**)
{
  UnitTest t (3);

  Composite c1;
  c1.add ("left",  2, Color ());
  c1.add ("right", 4, Color ());
  t.is (c1.str (), "  leright", "Composite left/2 + right/4 --> '  leright'");

  Composite c2;
  c2.add ("left",  2, Color ("white on red"));
  c2.add ("right", 4, Color ("white on blue"));
  t.diag (c2.str ());

  Composite c3;
  c3.add ("aaaaaaaaaa",  2, Color ());
  c3.add ("bbbbb",       5, Color ());
  c3.add ("c",          15, Color ());
  t.is (c3.str (), "  aaabbbbbaa   c", "Composite aaaaaaaaaa/2 + bbbbb/5 + c/15 --> '  aaabbbbbaa   c'");

  Composite c4;
  c4.add ("aaaaaaaaaa",  2, Color ("white on red"));
  c4.add ("bbbbb",       5, Color ("white on blue"));
  c4.add ("c",          15, Color ("white on green"));
  t.diag (c4.str ());

  Composite c5;
  c5.add ("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 0, Color ());
  c5.add ("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",   1, Color ());
  c5.add ("ccccccccccccccccccccccccccccccccccccccccccccccc",     2, Color ());
  c5.add ("ddddddddddddddddddddddddddddddddddddddddddddd",       3, Color ());
  c5.add ("eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",         4, Color ());
  c5.add ("fffffffffffffffffffffffffffffffffffffffff",           5, Color ());
  c5.add ("ggggggggggggggggggggggggggggggggggggggg",             6, Color ());
  c5.add ("hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh",               7, Color ());
  c5.add ("iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii",                 8, Color ());
  c5.add ("jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",                   9, Color ());
  c5.add ("kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk",                    10, Color ());
  c5.add ("lllllllllllllllllllllllllllll",                      11, Color ());
  c5.add ("mmmmmmmmmmmmmmmmmmmmmmmmmmm",                        12, Color ());
  c5.add ("nnnnnnnnnnnnnnnnnnnnnnnnn",                          13, Color ());
  c5.add ("ooooooooooooooooooooooo",                            14, Color ());
  c5.add ("ppppppppppppppppppppp",                              15, Color ());
  c5.add ("qqqqqqqqqqqqqqqqqqq",                                16, Color ());
  c5.add ("rrrrrrrrrrrrrrrrr",                                  17, Color ());
  c5.add ("sssssssssssssss",                                    18, Color ());
  c5.add ("ttttttttttttt",                                      19, Color ());
  c5.add ("uuuuuuuuuuu",                                        20, Color ());
  c5.add ("vvvvvvvvv",                                          21, Color ());
  c5.add ("wwwwwww",                                            22, Color ());
  c5.add ("xxxxx",                                              23, Color ());
  c5.add ("yyy",                                                24, Color ());
  c5.add ("z",                                                  25, Color ());
  t.is (c5.str (), "abcdefghijklmnopqrstuvwxyzyxwvutsrqponmlkjihgfedcba", "Composite ... --> 'abcdefghijklmnopqrstuvwxyzyxwvutsrqponmlkjihgfedcba'");

  Composite c6;
  c6.add ("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 0, Color ("blue on gray0"));
  c6.add ("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",   1, Color ("blue on gray1"));
  c6.add ("ccccccccccccccccccccccccccccccccccccccccccccccc",     2, Color ("blue on gray2"));
  c6.add ("ddddddddddddddddddddddddddddddddddddddddddddd",       3, Color ("blue on gray3"));
  c6.add ("eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",         4, Color ("blue on gray4"));
  c6.add ("fffffffffffffffffffffffffffffffffffffffff",           5, Color ("blue on gray5"));
  c6.add ("ggggggggggggggggggggggggggggggggggggggg",             6, Color ("blue on gray6"));
  c6.add ("hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh",               7, Color ("blue on gray7"));
  c6.add ("iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii",                 8, Color ("blue on gray8"));
  c6.add ("jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",                   9, Color ("blue on gray9"));
  c6.add ("kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk",                    10, Color ("blue on gray10"));
  c6.add ("lllllllllllllllllllllllllllll",                      11, Color ("blue on gray11"));
  c6.add ("mmmmmmmmmmmmmmmmmmmmmmmmmmm",                        12, Color ("blue on gray12"));
  c6.add ("nnnnnnnnnnnnnnnnnnnnnnnnn",                          13, Color ("blue on gray13"));
  c6.add ("ooooooooooooooooooooooo",                            14, Color ("blue on gray14"));
  c6.add ("ppppppppppppppppppppp",                              15, Color ("blue on gray15"));
  c6.add ("qqqqqqqqqqqqqqqqqqq",                                16, Color ("blue on gray16"));
  c6.add ("rrrrrrrrrrrrrrrrr",                                  17, Color ("blue on gray17"));
  c6.add ("sssssssssssssss",                                    18, Color ("blue on gray18"));
  c6.add ("ttttttttttttt",                                      19, Color ("blue on gray19"));
  c6.add ("uuuuuuuuuuu",                                        20, Color ("blue on gray20"));
  c6.add ("vvvvvvvvv",                                          21, Color ("blue on gray21"));
  c6.add ("wwwwwww",                                            22, Color ("blue on gray22"));
  c6.add ("xxxxx",                                              23, Color ("blue on gray23"));
  c6.add ("yyy",                                                24, Color ("blue on bright white"));
  c6.add ("z",                                                  25, Color ("blue on bright white"));
  t.diag (c6.str ());

  return 0;
}

////////////////////////////////////////////////////////////////////////////////

