// CellList is a 3D domain decomposition library.
// Copyright (C) 2011-2015 The CellList Development Team
//
// This file is part of CellList.
//
// CellList is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or (at your option) any later version.
//
// CellList is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>
//
//--


#include <stdexcept>
#include <gtest/gtest.h>
#include <cstdlib>
#include <cmath>
#include "celllists/cell.h"

// Helper functions
// ================

//! Fills an array of doubles with random numbers in range ]-0.5*scale, 0.5*scale]
void fill_random_double(double* array, size_t size, unsigned int seed, double scale=1) {
    srand(seed);
    for (int i=0; i<size; i++)
        array[i] = (rand() + 1.0)/(RAND_MAX + 1.0) - 0.5;
}

//! Fills an array of int with random numbers in range [-range, range]
void fill_random_int(int* array, size_t size, unsigned int seed, int range) {
    srand(seed);
    for (int i=0; i<size; i++)
        array[i] = (rand() % (2*range+1))-range;
}

Cell* create_random_cell_nvec(int nvec, unsigned int seed, double scale=1) {
    double rvecs[nvec*3];
    while (true) {
        try {
            fill_random_double(rvecs, nvec*3, seed, scale);
            return new Cell(rvecs, nvec);
        } catch (singular_cell_vectors) {}
    }
}


// Fixtures
// ========

class CellTest : public ::testing::Test {
    public:
        int nvec;
        Cell* mycell;
        double myrvecs[9];
        double singrvecs[9];
        Cell* create_random_cell(unsigned int seed, double scale=1) {
            Cell* cell = create_random_cell_nvec(nvec, seed, scale);
        }
        virtual void SetUp() = 0;
        void set_up_data() {
            // Example tests
            std::fill(myrvecs, myrvecs+9, 0);
            myrvecs[0] = 2;
            myrvecs[4] = 1;
            myrvecs[8] = 4;
            if (nvec == 2) {
                myrvecs[4] = 0.0;
                myrvecs[5] = 4.0;
            }
            mycell = new Cell(myrvecs, nvec);
            // Singular cell vectors
            std::fill(singrvecs, singrvecs+9, 0);
            if (nvec > 1) {
                singrvecs[0] = 1.0;
                singrvecs[3] = 0.5;
            }
            if (nvec == 3) {
                singrvecs[3] = 0.0;
                singrvecs[4] = 2.0;
                singrvecs[6] = 0.5;
                singrvecs[7] = 0.8;
            }
        }
        virtual void TearDown() {
            delete mycell;
        }
};

class CellTestP : public CellTest,
                  public ::testing::WithParamInterface<int> {
    public:
        virtual void SetUp() {
            nvec = GetParam();
            set_up_data();
        }
};

class CellTest1 : public CellTest {
    public:
        virtual void SetUp() {
            nvec = 1;
            set_up_data();
        }
};


class CellTest2 : public CellTest {
    public:
        virtual void SetUp() {
            nvec = 2;
            set_up_data();
        }
};


class CellTest3 : public CellTest {
    public:
        virtual void SetUp() {
            nvec = 3;
            set_up_data();
        }
};


// Tests grouped by main method being tested
// =========================================

// Constructor
// -----------

TEST_P(CellTestP, constructor_singular) {
    EXPECT_THROW(Cell cell(singrvecs, GetParam()), singular_cell_vectors);
}

TEST_F(CellTest3, constructor_nvec_negative) {
    double rvecs[9] = {1, 0, 1, 0, 1, 0, 0.5, 0.5, 0};
    EXPECT_THROW(Cell cell(rvecs, -1), std::domain_error);
}

TEST_F(CellTest3, constructor_nvec_too_large) {
    double rvecs[9] = {1, 0, 1, 0, 1, 0, 0.5, 0.5, 0};
    EXPECT_THROW(Cell cell(rvecs, 4), std::domain_error);
}

TEST_P(CellTestP, constructor_simple) {
    double rvecs[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
    Cell cell(rvecs, nvec);
    EXPECT_EQ(cell.get_nvec(), nvec);
    EXPECT_EQ(cell.get_rvec(0, 0), 1.0);
    EXPECT_EQ(cell.get_rvec(0, 1), 0.0);
    EXPECT_EQ(cell.get_rvec(0, 1), 0.0);
    EXPECT_EQ(cell.get_rvec(1, 0), 0.0);
    EXPECT_EQ(cell.get_rvec(1, 1), 1.0);
    EXPECT_EQ(cell.get_rvec(1, 2), 0.0);
    EXPECT_EQ(cell.get_rvec(2, 0), 0.0);
    EXPECT_EQ(cell.get_rvec(2, 1), 0.0);
    EXPECT_EQ(cell.get_rvec(2, 2), 1.0);
    EXPECT_EQ(cell.get_gvec(0, 0), 1.0);
    EXPECT_EQ(cell.get_gvec(0, 1), 0.0);
    EXPECT_EQ(cell.get_gvec(0, 2), 0.0);
    EXPECT_EQ(cell.get_gvec(1, 0), 0.0);
    EXPECT_EQ(cell.get_gvec(1, 1), 1.0);
    EXPECT_EQ(cell.get_gvec(1, 2), 0.0);
    EXPECT_EQ(cell.get_gvec(2, 0), 0.0);
    EXPECT_EQ(cell.get_gvec(2, 1), 0.0);
    EXPECT_EQ(cell.get_gvec(2, 2), 1.0);
    EXPECT_EQ(cell.get_volume(), nvec > 0);
    EXPECT_EQ(cell.get_rspacing(0), 1.0);
    EXPECT_EQ(cell.get_rspacing(1), 1.0);
    EXPECT_EQ(cell.get_rspacing(2), 1.0);
    EXPECT_EQ(cell.get_gspacing(0), 1.0);
    EXPECT_EQ(cell.get_gspacing(1), 1.0);
    EXPECT_EQ(cell.get_gspacing(2), 1.0);
    EXPECT_EQ(cell.is_cubic(), true);
    EXPECT_EQ(cell.is_cuboid(), true);
}


// wrap
// ----

TEST_F(CellTest1, wrap_example) {
    double delta[3] = {2.5, 4.3, 3.0};
    mycell->wrap(delta);
    EXPECT_EQ(delta[0], 0.5);
    EXPECT_EQ(delta[1], 4.3);
    EXPECT_EQ(delta[2], 3.0);
}

TEST_F(CellTest2, wrap_example) {
    double delta[3] = {2.0, 5.3, 3.0};
    mycell->wrap(delta);
    EXPECT_EQ(delta[0], 0.0);
    EXPECT_EQ(delta[1], 5.3);
    EXPECT_EQ(delta[2], -1.0);
}

TEST_F(CellTest3, wrap_example) {
    double delta[3] = {2.0, 0.3, 3.0};
    mycell->wrap(delta);
    EXPECT_EQ(delta[0], 0.0);
    EXPECT_EQ(delta[1], 0.3);
    EXPECT_EQ(delta[2], -1.0);
}

TEST_F(CellTest1, wrap_edges) {
    double delta[3] = {-1.0, -0.5, -2.0};
    mycell->wrap(delta);
    EXPECT_EQ(delta[0],  1.0);
    EXPECT_EQ(delta[1], -0.5);
    EXPECT_EQ(delta[2], -2.0);
}

TEST_F(CellTest2, wrap_edges) {
    double delta[3] = {-1.0, -0.5, -2.0};
    mycell->wrap(delta);
    EXPECT_EQ(delta[0],  1.0);
    EXPECT_EQ(delta[1], -0.5);
    EXPECT_EQ(delta[2],  2.0);
}

TEST_F(CellTest3, wrap_edges) {
    double delta[3] = {-1.0, -0.5, -2.0};
    mycell->wrap(delta);
    EXPECT_EQ(delta[0], 1.0);
    EXPECT_EQ(delta[1], 0.5);
    EXPECT_EQ(delta[2], 2.0);
}

TEST_P(CellTestP, wrap_random) {
    for (int irep=0; irep < 100; irep++) {
        Cell* cell = create_random_cell(irep);
        double delta[3];
        fill_random_double(delta, 3, 11, 5.0);
        double frac[3];
        cell->wrap(delta);
        cell->to_frac(delta, frac);
        for (int ivec=0; ivec<=3; ivec++) {
            EXPECT_LT(frac[0], 0.5);
            EXPECT_GE(frac[0], -0.5);
        }
        delete cell;
    }
}

TEST_P(CellTestP, wrap_consistency) {
    for (int irep=0; irep < 100; irep++) {
        Cell* cell = create_random_cell(irep);
        int coeffs[nvec];
        fill_random_int(coeffs, nvec, irep, 5);
        double frac[3];
        double cart1[3];
        double cart2[3];
        fill_random_double(frac, 3, 11, 1.0);
        cell->to_cart(frac, cart1);
        cell->to_cart(frac, cart2);
        cell->add_rvec(cart2, coeffs);
        cell->wrap(cart2);
        EXPECT_NEAR(cart1[0], cart2[0], 1e-10);
        EXPECT_NEAR(cart1[1], cart2[1], 1e-10);
        EXPECT_NEAR(cart1[2], cart2[2], 1e-10);
        delete cell;
    }
}

// to_frac and to_cart
// -------------------

TEST_F(CellTest1, to_frac_example) {
    double cart[3] = {2.5, 4.3, 3.0};
    double frac[3];
    mycell->to_frac(cart, frac);
    EXPECT_NEAR(frac[0], 1.25, 1e-10);
    EXPECT_NEAR(frac[1], 4.3, 1e-10);
    EXPECT_NEAR(frac[2], 3.0, 1e-10);
}

TEST_F(CellTest2, to_frac_example) {
    double cart[3] = {2.5, 4.3, 3.0};
    double frac[3];
    mycell->to_frac(cart, frac);
    EXPECT_NEAR(frac[0], 1.25, 1e-10);
    EXPECT_NEAR(frac[1], 0.75, 1e-10);
    EXPECT_NEAR(frac[2], -4.3, 1e-10);
}

TEST_F(CellTest3, to_frac_example) {
    double cart[3] = {2.5, 4.3, 3.0};
    double frac[3];
    mycell->to_frac(cart, frac);
    EXPECT_NEAR(frac[0], 1.25, 1e-10);
    EXPECT_NEAR(frac[1], 4.3, 1e-10);
    EXPECT_NEAR(frac[2], 0.75, 1e-10);
}

TEST_F(CellTest1, to_cart_example) {
    double frac[3] = {0.5, 0.2, -1.5};
    double cart[3];
    mycell->to_cart(frac, cart);
    EXPECT_NEAR(cart[0], 1.0, 1e-10);
    EXPECT_NEAR(cart[1], 0.2, 1e-10);
    EXPECT_NEAR(cart[2], -1.5, 1e-10);
}

TEST_F(CellTest2, to_cart_example) {
    double frac[3] = {0.5, 0.2, -1.5};
    double cart[3];
    mycell->to_cart(frac, cart);
    EXPECT_NEAR(cart[0], 1.0, 1e-10);
    EXPECT_NEAR(cart[1], 1.5, 1e-10);
    EXPECT_NEAR(cart[2], 0.8, 1e-10);
}

TEST_F(CellTest3, to_cart_example) {
    double frac[3] = {0.5, 0.2, -1.5};
    double cart[3];
    mycell->to_cart(frac, cart);
    EXPECT_NEAR(cart[0], 1.0, 1e-10);
    EXPECT_NEAR(cart[1], 0.2, 1e-10);
    EXPECT_NEAR(cart[2], -6.0, 1e-10);
}

TEST_P(CellTestP, to_cart_to_frac_consistency) {
    for (int irep=0; irep < 100; irep++) {
        Cell* cell = create_random_cell(irep);
        double frac[3];
        double cart1[3];
        double cart2[3];
        fill_random_double(cart1, 3, 11, 5.0);
        cell->to_frac(cart1, frac);
        cell->to_cart(frac, cart2);
        EXPECT_NEAR(cart1[0], cart2[0], 1e-10);
        EXPECT_NEAR(cart1[1], cart2[1], 1e-10);
        EXPECT_NEAR(cart1[2], cart2[2], 1e-10);
        delete cell;
    }
}

// g_lincomb and dot_rvecs
// -----------------------

TEST_F(CellTest1, g_lincomb_example) {
    double coeffs[3] = {2.5, 4.3, 3.0};
    double gvec[3];
    mycell->g_lincomb(coeffs, gvec);
    EXPECT_NEAR(gvec[0], 1.25, 1e-10);
    EXPECT_NEAR(gvec[1], 4.3, 1e-10);
    EXPECT_NEAR(gvec[2], 3.0, 1e-10);
}

TEST_F(CellTest2, g_lincomb_example) {
    double coeffs[3] = {2.5, 4.3, 3.0};
    double gvec[3];
    mycell->g_lincomb(coeffs, gvec);
    EXPECT_NEAR(gvec[0],  1.25, 1e-10);
    EXPECT_NEAR(gvec[1], -3.0, 1e-10);
    EXPECT_NEAR(gvec[2],  1.075, 1e-10);
}

TEST_F(CellTest3, g_lincomb_example) {
    double coeffs[3] = {2.5, 4.3, 3.0};
    double gvec[3];
    mycell->g_lincomb(coeffs, gvec);
    EXPECT_NEAR(gvec[0], 1.25, 1e-10);
    EXPECT_NEAR(gvec[1], 4.3, 1e-10);
    EXPECT_NEAR(gvec[2], 0.75, 1e-10);
}

TEST_F(CellTest1, dot_rvecs_example) {
    double gvec[3] = {0.5, 0.2, -1.5};
    double dots[3];
    mycell->dot_rvecs(gvec, dots);
    EXPECT_NEAR(dots[0],  1.0, 1e-10);
    EXPECT_NEAR(dots[1],  0.2, 1e-10);
    EXPECT_NEAR(dots[2], -1.5, 1e-10);
}

TEST_F(CellTest2, dot_rvecs_example) {
    double gvec[3] = {0.5, 0.2, -1.5};
    double dots[3];
    mycell->dot_rvecs(gvec, dots);
    EXPECT_NEAR(dots[0],  1.0, 1e-10);
    EXPECT_NEAR(dots[1], -6.0, 1e-10);
    EXPECT_NEAR(dots[2], -0.2, 1e-10);
}

TEST_F(CellTest3, dot_rvecs_example) {
    double gvec[3] = {0.5, 0.2, -1.5};
    double dots[3];
    mycell->dot_rvecs(gvec, dots);
    EXPECT_NEAR(dots[0], 1.0, 1e-10);
    EXPECT_NEAR(dots[1], 0.2, 1e-10);
    EXPECT_NEAR(dots[2], -6.0, 1e-10);
}

TEST_P(CellTestP, g_lincomb_dot_rvecs_consistency) {
    for (int irep=0; irep < 100; irep++) {
        Cell* cell = create_random_cell(irep);
        double coeffs[3];
        double gvec[3];
        double dots[3];
        fill_random_double(coeffs, 3, 11, 5.0);
        cell->g_lincomb(coeffs, gvec);
        cell->dot_rvecs(gvec, dots);
        EXPECT_NEAR(coeffs[0], dots[0], 1e-10);
        EXPECT_NEAR(coeffs[1], dots[1], 1e-10);
        EXPECT_NEAR(coeffs[2], dots[2], 1e-10);
        delete cell;
    }
}

// add_rvec
// --------

TEST_P(CellTestP, add_rvec_consistency) {
    for (int irep=0; irep < 100; irep++) {
        Cell* cell = create_random_cell(irep);
        int coeffs[nvec];
        fill_random_int(coeffs, nvec, irep, 5);
        double cart1[3];
        double cart2[3];
        double frac1[3];
        double frac2[3];
        fill_random_double(cart1, 3, 11, 10.0);
        cart2[0] = cart1[0];
        cart2[1] = cart1[1];
        cart2[2] = cart1[2];
        cell->add_rvec(cart2, coeffs);
        cell->to_frac(cart1, frac1);
        cell->to_frac(cart2, frac2);
        for (int ivec=0; ivec < nvec; ivec++) {
            EXPECT_NEAR(frac2[ivec] - frac1[ivec], coeffs[ivec], 1e-10);
        }
        for (int ivec=nvec; ivec < 3; ivec++) {
            EXPECT_NEAR(frac2[ivec] - frac1[ivec], 0.0, 1e-10);
        }
        delete cell;
    }
}

// The getters
// -----------

// get_nvec() is already tested above

TEST_P(CellTestP, get_rvec) {
    double rvecs[nvec*3];
    Cell* cell = NULL;
    while (true) {
        try {
            fill_random_double(rvecs, nvec*3, 1487, 2.0);
            cell = new Cell(rvecs, nvec);
            break;
        } catch (singular_cell_vectors) {}
    }
    for (int ivec=0; ivec < nvec; ivec++) {
        EXPECT_EQ(cell->get_rvec(ivec, 0), rvecs[3*ivec+0]);
        EXPECT_EQ(cell->get_rvec(ivec, 1), rvecs[3*ivec+1]);
        EXPECT_EQ(cell->get_rvec(ivec, 2), rvecs[3*ivec+2]);
    }
    delete cell;
}

TEST_P(CellTestP, get_domain) {
    double rvecs[nvec*3];
    Cell* cell = NULL;
    fill_random_double(rvecs, nvec*3, 1487, 2.0);
    cell = new Cell(rvecs, nvec);
    EXPECT_THROW(cell->get_rvec(-1, 0), std::domain_error);
    EXPECT_THROW(cell->get_rvec(3, 0), std::domain_error);
    EXPECT_THROW(cell->get_rvec(0, -1), std::domain_error);
    EXPECT_THROW(cell->get_rvec(0, 3), std::domain_error);
    EXPECT_THROW(cell->get_gvec(-1, 0), std::domain_error);
    EXPECT_THROW(cell->get_gvec(3, 0), std::domain_error);
    EXPECT_THROW(cell->get_gvec(0, -1), std::domain_error);
    EXPECT_THROW(cell->get_gvec(0, 3), std::domain_error);
    EXPECT_THROW(cell->get_rlength(-1), std::domain_error);
    EXPECT_THROW(cell->get_rlength(3), std::domain_error);
    EXPECT_THROW(cell->get_glength(-1), std::domain_error);
    EXPECT_THROW(cell->get_glength(3), std::domain_error);
    EXPECT_THROW(cell->get_rspacing(-1), std::domain_error);
    EXPECT_THROW(cell->get_rspacing(3), std::domain_error);
    EXPECT_THROW(cell->get_gspacing(-1), std::domain_error);
    EXPECT_THROW(cell->get_gspacing(3), std::domain_error);
    delete cell;
}

TEST_F(CellTest1, get_example) {
    EXPECT_NEAR(mycell->get_gvec(0, 0), 0.5, 1e-10);
    EXPECT_NEAR(mycell->get_gvec(0, 1), 0.0, 1e-10);
    EXPECT_NEAR(mycell->get_gvec(0, 2), 0.0, 1e-10);
    EXPECT_NEAR(mycell->get_gvec(1, 0), 0.0, 1e-10);
    EXPECT_NEAR(mycell->get_gvec(1, 1), 1.0, 1e-10);
    EXPECT_NEAR(mycell->get_gvec(1, 2), 0.0, 1e-10);
    EXPECT_NEAR(mycell->get_gvec(2, 0), 0.0, 1e-10);
    EXPECT_NEAR(mycell->get_gvec(2, 1), 0.0, 1e-10);
    EXPECT_NEAR(mycell->get_gvec(2, 2), 1.0, 1e-10);
    EXPECT_NEAR(mycell->get_volume(), 2.0, 1e-10);
    EXPECT_NEAR(mycell->get_rlength(0), 2.0, 1e-10);
    EXPECT_NEAR(mycell->get_rlength(1), 1.0, 1e-10);
    EXPECT_NEAR(mycell->get_rlength(2), 1.0, 1e-10);
    EXPECT_NEAR(mycell->get_rspacing(0), 2.0, 1e-10);
    EXPECT_NEAR(mycell->get_rspacing(1), 1.0, 1e-10);
    EXPECT_NEAR(mycell->get_rspacing(2), 1.0, 1e-10);
    EXPECT_NEAR(mycell->get_glength(0), 0.5, 1e-10);
    EXPECT_NEAR(mycell->get_glength(1), 1.0, 1e-10);
    EXPECT_NEAR(mycell->get_glength(2), 1.0, 1e-10);
    EXPECT_NEAR(mycell->get_gspacing(0), 0.5, 1e-10);
    EXPECT_NEAR(mycell->get_gspacing(1), 1.0, 1e-10);
    EXPECT_NEAR(mycell->get_gspacing(2), 1.0, 1e-10);
}

TEST_F(CellTest2, get_example) {
    EXPECT_NEAR(mycell->get_gvec(0, 0),  0.5, 1e-10);
    EXPECT_NEAR(mycell->get_gvec(0, 1),  0.0, 1e-10);
    EXPECT_NEAR(mycell->get_gvec(0, 2),  0.0, 1e-10);
    EXPECT_NEAR(mycell->get_gvec(1, 0),  0.0, 1e-10);
    EXPECT_NEAR(mycell->get_gvec(1, 1),  0.0, 1e-10);
    EXPECT_NEAR(mycell->get_gvec(1, 2),  0.25, 1e-10);
    EXPECT_NEAR(mycell->get_gvec(2, 0),  0.0, 1e-10);
    EXPECT_NEAR(mycell->get_gvec(2, 1), -1.0, 1e-10);
    EXPECT_NEAR(mycell->get_gvec(2, 2),  0.0, 1e-10);
    EXPECT_NEAR(mycell->get_volume(), 8.0, 1e-10);
    EXPECT_NEAR(mycell->get_rlength(0), 2.0, 1e-10);
    EXPECT_NEAR(mycell->get_rlength(1), 4.0, 1e-10);
    EXPECT_NEAR(mycell->get_rlength(2), 1.0, 1e-10);
    EXPECT_NEAR(mycell->get_rspacing(0), 2.0, 1e-10);
    EXPECT_NEAR(mycell->get_rspacing(1), 4.0, 1e-10);
    EXPECT_NEAR(mycell->get_rspacing(2), 1.0, 1e-10);
    EXPECT_NEAR(mycell->get_glength(0), 0.5, 1e-10);
    EXPECT_NEAR(mycell->get_glength(1), 0.25, 1e-10);
    EXPECT_NEAR(mycell->get_glength(2), 1.0, 1e-10);
    EXPECT_NEAR(mycell->get_gspacing(0), 0.5, 1e-10);
    EXPECT_NEAR(mycell->get_gspacing(1), 0.25, 1e-10);
    EXPECT_NEAR(mycell->get_gspacing(2), 1.0, 1e-10);
}

TEST_F(CellTest3, get_example) {
    EXPECT_NEAR(mycell->get_gvec(0, 0), 0.5, 1e-10);
    EXPECT_NEAR(mycell->get_gvec(0, 1), 0.0, 1e-10);
    EXPECT_NEAR(mycell->get_gvec(0, 2), 0.0, 1e-10);
    EXPECT_NEAR(mycell->get_gvec(1, 0), 0.0, 1e-10);
    EXPECT_NEAR(mycell->get_gvec(1, 1), 1.0, 1e-10);
    EXPECT_NEAR(mycell->get_gvec(1, 2), 0.0, 1e-10);
    EXPECT_NEAR(mycell->get_gvec(2, 0), 0.0, 1e-10);
    EXPECT_NEAR(mycell->get_gvec(2, 1), 0.0, 1e-10);
    EXPECT_NEAR(mycell->get_gvec(2, 2), 0.25, 1e-10);
    EXPECT_NEAR(mycell->get_volume(), 8.0, 1e-10);
    EXPECT_NEAR(mycell->get_rlength(0), 2.0, 1e-10);
    EXPECT_NEAR(mycell->get_rlength(1), 1.0, 1e-10);
    EXPECT_NEAR(mycell->get_rlength(2), 4.0, 1e-10);
    EXPECT_NEAR(mycell->get_rspacing(0), 2.0, 1e-10);
    EXPECT_NEAR(mycell->get_rspacing(1), 1.0, 1e-10);
    EXPECT_NEAR(mycell->get_rspacing(2), 4.0, 1e-10);
    EXPECT_NEAR(mycell->get_glength(0), 0.5, 1e-10);
    EXPECT_NEAR(mycell->get_glength(1), 1.0, 1e-10);
    EXPECT_NEAR(mycell->get_glength(2), 0.25, 1e-10);
    EXPECT_NEAR(mycell->get_gspacing(0), 0.5, 1e-10);
    EXPECT_NEAR(mycell->get_gspacing(1), 1.0, 1e-10);
    EXPECT_NEAR(mycell->get_gspacing(2), 0.25, 1e-10);
}

// is_cubic and is_cuboid
// ----------------------

TEST_P(CellTestP, cubic_cuboid_random) {
    for (int irep=0; irep < 100; irep++) {
        Cell* cell = create_random_cell(irep);
        EXPECT_FALSE(cell->is_cubic());
        EXPECT_FALSE(cell->is_cuboid());
        delete cell;
    }
}

TEST_F(CellTest1, cubic_cuboid_example) {
    EXPECT_TRUE(mycell->is_cubic());
    EXPECT_TRUE(mycell->is_cuboid());
}

TEST_F(CellTest2, cubic_cuboid_example) {
    EXPECT_FALSE(mycell->is_cubic());
    EXPECT_FALSE(mycell->is_cuboid());
}

TEST_F(CellTest3, cubic_cuboid_example) {
    EXPECT_FALSE(mycell->is_cubic());
    EXPECT_TRUE(mycell->is_cuboid());
}

// set_ranges_rcut
// ---------------

TEST_F(CellTest1, set_ranges_rcut_example) {
    double center[3] = {6.3, 0.2, -0.8};
    int ranges_begin[1];
    int ranges_end[1];
    int ncell = 0;
    ncell = mycell->set_ranges_rcut(center, 1.0, ranges_begin, ranges_end);
    EXPECT_EQ(ncell, 2);
    EXPECT_EQ(ranges_begin[0], 2);
    EXPECT_EQ(ranges_end[0], 4);
    ncell = mycell->set_ranges_rcut(center, 2.0, ranges_begin, ranges_end);
    EXPECT_EQ(ncell, 3);
    EXPECT_EQ(ranges_begin[0], 2);
    EXPECT_EQ(ranges_end[0], 5);
    ncell = mycell->set_ranges_rcut(center, 3.0, ranges_begin, ranges_end);
    EXPECT_EQ(ncell, 4);
    EXPECT_EQ(ranges_begin[0], 1);
    EXPECT_EQ(ranges_end[0], 5);
}

TEST_F(CellTest1, set_ranges_rcut_edge) {
    double center[3] = {2.0, 0.2, -0.8};
    int ranges_begin[1];
    int ranges_end[1];
    int ncell = 0;
    ncell = mycell->set_ranges_rcut(center, 1.0, ranges_begin, ranges_end);
    EXPECT_EQ(ncell, 2);
    EXPECT_EQ(ranges_begin[0], 0);
    EXPECT_EQ(ranges_end[0], 2);
    ncell = mycell->set_ranges_rcut(center, 2.0, ranges_begin, ranges_end);
    EXPECT_EQ(ncell, 2);
    EXPECT_EQ(ranges_begin[0], 0);
    EXPECT_EQ(ranges_end[0], 2);
    ncell = mycell->set_ranges_rcut(center, 3.0, ranges_begin, ranges_end);
    EXPECT_EQ(ncell, 4);
    EXPECT_EQ(ranges_begin[0], -1);
    EXPECT_EQ(ranges_end[0], 3);
}

TEST_F(CellTest2, set_ranges_rcut_example) {
    double center[3] = {6.3, 0.2, -5.0};
    int ranges_begin[2];
    int ranges_end[2];
    int ncell = 0;
    ncell = mycell->set_ranges_rcut(center, 1.1, ranges_begin, ranges_end);
    EXPECT_EQ(ncell, 2*2);
    EXPECT_EQ(ranges_begin[0], 2);
    EXPECT_EQ(ranges_begin[1], -2);
    EXPECT_EQ(ranges_end[0], 4);
    EXPECT_EQ(ranges_end[1], 0);
}

TEST_F(CellTest2, set_ranges_rcut_edge) {
    double center[3] = {4.0, 0.2, -2.0};
    int ranges_begin[2];
    int ranges_end[2];
    int ncell = 0;
    ncell = mycell->set_ranges_rcut(center, 2.0, ranges_begin, ranges_end);
    EXPECT_EQ(ncell, 2);
    EXPECT_EQ(ranges_begin[0], 1);
    EXPECT_EQ(ranges_begin[1], -1);
    EXPECT_EQ(ranges_end[0], 3);
    EXPECT_EQ(ranges_end[1], 0);
}

TEST_F(CellTest3, set_ranges_rcut_example) {
    double center[3] = {6.3, 2.2, -5.8};
    int ranges_begin[3];
    int ranges_end[3];
    int ncell = 0;
    ncell = mycell->set_ranges_rcut(center, 1.0, ranges_begin, ranges_end);
    EXPECT_EQ(ncell, 2*3*1);
    EXPECT_EQ(ranges_begin[0], 2);
    EXPECT_EQ(ranges_begin[1], 1);
    EXPECT_EQ(ranges_begin[2], -2);
    EXPECT_EQ(ranges_end[0], 4);
    EXPECT_EQ(ranges_end[1], 4);
    EXPECT_EQ(ranges_end[2], -1);
}

TEST_F(CellTest3, set_ranges_rcut_edge) {
    double center[3] = {10.0, -2.0, -6.0};
    int ranges_begin[3];
    int ranges_end[3];
    int ncell = 0;
    ncell = mycell->set_ranges_rcut(center, 2.0, ranges_begin, ranges_end);
    EXPECT_EQ(ncell, 2*4*1);
    EXPECT_EQ(ranges_begin[0], 4);
    EXPECT_EQ(ranges_begin[1], -4);
    EXPECT_EQ(ranges_begin[2], -2);
    EXPECT_EQ(ranges_end[0], 6);
    EXPECT_EQ(ranges_end[1], 0);
    EXPECT_EQ(ranges_end[2], -1);
}

TEST_P(CellTestP, set_ranges_rcut_domain) {
    double center[3] = {6.3, 2.2, -5.8};
    int ranges_begin[nvec];
    int ranges_end[nvec];
    EXPECT_THROW(mycell->set_ranges_rcut(center, -1.0, ranges_begin, ranges_end), std::domain_error);
    EXPECT_THROW(mycell->set_ranges_rcut(center, 0.0, ranges_begin, ranges_end), std::domain_error);
}

TEST_P(CellTestP, set_ranges_rcut_random) {
    for (int icell=0; icell < 100; icell++) {
        Cell* cell = create_random_cell(icell);
        double center[3];
        int ranges_begin[nvec];
        int ranges_end[nvec];
        double rcut = 0.3*(icell+1);
        fill_random_double(center, 3, icell+2, 5.0);
        cell->set_ranges_rcut(center, rcut, ranges_begin, ranges_end);
        for (int ipoint=0; ipoint < 1000; ipoint++) {
            // Make a random point that is uniformly distributed within a sphere, but
            // that has a high probability of setting on the edge.
            double point[3];
            double frac[3];
            double norm;
            fill_random_double(point, 3, ipoint+icell*1000, 1.0);
            norm = sqrt(point[0]*point[0] + point[1]*point[1] + point[2]*point[2]);
            if (norm > 1) {
                point[0] /= norm;
                point[1] /= norm;
                point[2] /= norm;
            }
            point[0] = center[0] + rcut*point[0];
            point[1] = center[1] + rcut*point[1];
            point[2] = center[2] + rcut*point[2];
            cell->to_frac(point, frac);
            for (int ivec=0; ivec < nvec; ivec++) {
                EXPECT_LE(ranges_begin[ivec], frac[ivec]);
                EXPECT_GE(ranges_end[ivec], frac[ivec]);
            }
        }
        delete cell;
    }
}

// select_inside
// -------------

/* TODO

- Unit tests for different variants: shape, pbc
- rename longs -> ints
- change order of first two arguments of all EXPECT and ASSERT calls.

*/

TEST_F(CellTest1, select_inside_example) {
    // All the parameters
    double rcut = 5.0;
    double center[3] = {2.5, 3.4, -0.6};
    int shape[1] = {10};
    bool pbc[1] = {true};
    int indices[2];

    // Call
    int nselect = mycell->select_inside_rcut(center, rcut, shape, pbc, indices);

    // Check results
    // lower end: -2.5 (-2 #> 8)
    // upper end:  7.5 ( 8 #> 4) non-inclusive
    EXPECT_EQ(nselect, 1);
    EXPECT_EQ(indices[0], -2);
    EXPECT_EQ(indices[1], 4);
}


// Instantiation of parameterized tests
// ------------------------------------

INSTANTIATE_TEST_CASE_P(CellTest123, CellTestP, ::testing::Range(1, 4));


// smart_wrap
// ----------

TEST(SmartWrap, examples) {
    EXPECT_EQ(smart_wrap(-15, 5, true), 0);
    EXPECT_EQ(smart_wrap( -5, 5, true), 0);
    EXPECT_EQ(smart_wrap( -3, 5, true), 2);
    EXPECT_EQ(smart_wrap( -1, 5, true), 4);
    EXPECT_EQ(smart_wrap(  0, 5, true), 0);
    EXPECT_EQ(smart_wrap(  3, 5, true), 3);
    EXPECT_EQ(smart_wrap(  5, 5, true), 0);
    EXPECT_EQ(smart_wrap(  6, 5, true), 1);
    EXPECT_EQ(smart_wrap( 10, 5, true), 0);
    EXPECT_EQ(smart_wrap( 12, 5, true), 2);
    EXPECT_EQ(smart_wrap(-15, 5, false), -1);
    EXPECT_EQ(smart_wrap( -5, 5, false), -1);
    EXPECT_EQ(smart_wrap( -3, 5, false), -1);
    EXPECT_EQ(smart_wrap( -1, 5, false), -1);
    EXPECT_EQ(smart_wrap(  0, 5, false),  0);
    EXPECT_EQ(smart_wrap(  3, 5, false),  3);
    EXPECT_EQ(smart_wrap(  4, 5, false),  4);
    EXPECT_EQ(smart_wrap(  5, 5, false), -1);
    EXPECT_EQ(smart_wrap(  6, 5, false), -1);
    EXPECT_EQ(smart_wrap( 10, 5, false), -1);
    EXPECT_EQ(smart_wrap( 12, 5, false), -1);
}
