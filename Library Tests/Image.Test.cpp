#include "targetver.h"
#include "CppUnitTest.h"
#include "Image.hpp"
#include "Pixel.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace bpl::image;

namespace Microsoft
{
	namespace VisualStudio
	{
		namespace CppUnitTestFramework
		{
			template<>
			static std::wstring ToString<bpl::image::Pixel24>(const bpl::image::Pixel24& t)
			{
				std::wstringstream Stream;
				Stream << '(' << t.Red << ", " << t.Green << ", " << t.Blue << ')';
				return Stream.str();
			}
		}
	}
}

namespace LibraryTests
{
	TEST_CLASS(ImageTest)
	{
		Image<Pixel24> Object;

	public:
		TEST_METHOD_INITIALIZE(InitObject)
		{
			Object.Resize(3, 2);
			
			Object(0, 0) = { 255, 0, 0 };
			Object(1, 0) = { 0, 255, 0 };
			Object(2, 0) = { 0, 0, 255 };
			Object(0, 1) = { 255, 255, 255 };
			Object(1, 1) = { 127, 127, 127 };
			Object(2, 1) = { 0, 0, 127 };
		}

		TEST_METHOD(ArrayAccess)
		{
			Assert::AreEqual(Pixel24(127, 127, 127), Object(1, 1));
		}

		TEST_METHOD(BoundsChecking)
		{
			Assert::AreEqual(Pixel24(0, 0, 127), Object.at(2, 1));
			Assert::ExpectException<std::out_of_range>([&]{ Object.at(3, 1); });
			Assert::ExpectException<std::out_of_range>([&]{ Object.at(2, 2); });
		}

		TEST_METHOD(Dimensions)
		{
			Assert::AreEqual(std::size_t(3), Object.Width());
			Assert::AreEqual(std::size_t(2), Object.Height());
			Assert::AreEqual(std::size_t(24), Object.Depth());
			Assert::AreEqual(std::size_t(6), Object.Size());
		}

		TEST_METHOD(Content)
		{
			Assert::IsFalse(Object.Empty());
			Object.Clear();
			Assert::IsTrue(Object.Empty());
		}

		TEST_METHOD(Resize)
		{
			Object.Resize(5, 1);
			Assert::AreEqual(std::size_t(5), Object.Width());
			Assert::AreEqual(std::size_t(1), Object.Height());
		}

		TEST_METHOD(Apply)
		{
			Object.Apply([&](Image<Pixel24>::const_reference x) { return Pixel24(x.Red % 64, x.Blue % 64, x.Green % 64); });
			Assert::AreEqual(Pixel24(63, 63, 63), Object(1, 1));
		}

		TEST_METHOD(Fill)
		{
			Object.Fill(Pixel24(42, 69, 7));
			Assert::AreEqual(Pixel24(42, 69, 7), Object.at(1, 0));
		}

		TEST_METHOD(Swap)
		{
			using std::swap;

			Image<Pixel24> Temp(5, 5);
			swap(Temp, Object);

			Assert::AreEqual(std::size_t(3), Temp.Width());
			Assert::AreEqual(std::size_t(2), Temp.Height());
			Assert::AreEqual(std::size_t(5), Object.Width());
			Assert::AreEqual(std::size_t(5), Object.Height());
		}

		TEST_METHOD(Flip)
		{
			Image<Pixel24> Original(Object);

			Object.Flip(bpl::Axis::x);
			Object.Flip(bpl::Axis::x);
			for (std::size_t y = 0; y < Object.Height(); ++y)
				for (std::size_t x = 0; x < Object.Width(); ++x)
					Assert::AreEqual(Original(x, y), Object(x, y));

			Object.Flip(bpl::Axis::y);
			Object.Flip(bpl::Axis::y);
			for (std::size_t y = 0; y < Object.Height(); ++y)
				for (std::size_t x = 0; x < Object.Width(); ++x)
					Assert::AreEqual(Original(x, y), Object(x, y));

			Assert::ExpectException<std::out_of_range>(std::bind(&Image<Pixel24>::Flip, &Object, bpl::Axis::z));
		}

		TEST_METHOD(Mirror)
		{
			Object.Resize(3, 1);

			Image<Pixel24> ExpectedX(3, 2);
			ExpectedX(0, 0) = { 255, 0, 0 };
			ExpectedX(1, 0) = { 0, 255, 0 };
			ExpectedX(2, 0) = { 0, 0, 255 };
			ExpectedX(0, 1) = { 255, 0, 0 };
			ExpectedX(1, 1) = { 0, 255, 0 };
			ExpectedX(2, 1) = { 0, 0, 255 };

			Image<Pixel24> ExpectedY(6, 1);
			ExpectedY(0, 0) = { 255, 0, 0 };
			ExpectedY(1, 0) = { 0, 255, 0 };
			ExpectedY(2, 0) = { 0, 0, 255 };
			ExpectedY(3, 0) = { 0, 0, 255 };
			ExpectedY(4, 0) = { 0, 255, 0 };
			ExpectedY(5, 0) = { 255, 0, 0 };
			
			Object.Mirror(bpl::Axis::x);
			Assert::IsTrue(Object == ExpectedX);
			
			Object.Resize(3, 1);

			Object.Mirror(bpl::Axis::y);
			Assert::IsTrue(Object == ExpectedY);

			Assert::ExpectException<std::out_of_range>(std::bind(&Image<Pixel24>::Mirror, &Object, bpl::Axis::z));
		}

		TEST_METHOD(Rotate)
		{
			Image<Pixel24> Clone(Object);

			Image<Pixel24> Expected90(2, 3);
			Expected90(0, 0) = { 255, 255, 255 };
			Expected90(1, 0) = { 255, 0, 0 };
			Expected90(0, 1) = { 127, 127, 127 };
			Expected90(1, 1) = { 0, 255, 0 };
			Expected90(0, 2) = { 0, 0, 127 };
			Expected90(1, 2) = { 0, 0, 255 };

			Image<Pixel24> Expected180(3, 2);
			Expected180(0, 0) = { 0, 0, 127 };
			Expected180(1, 0) = { 127, 127, 127 };
			Expected180(2, 0) = { 255, 255, 255 };
			Expected180(0, 1) = { 0, 0, 255 };
			Expected180(1, 1) = { 0, 255, 0 };
			Expected180(2, 1) = { 255, 0, 0 };

			Image<Pixel24> Expected270(2, 3);
			Expected270(0, 0) = { 0, 0, 255 };
			Expected270(1, 0) = { 0, 0, 127 };
			Expected270(0, 1) = { 0, 255, 0 };
			Expected270(1, 1) = { 127, 127, 127 };
			Expected270(0, 2) = { 255, 0, 0 };
			Expected270(1, 2) = { 255, 255, 255 };

			Clone.Rotate(bpl::Rotation::_90);
			Assert::IsTrue(Clone == Expected90, L"(90 degrees)");

			Clone = Object;
			Clone.Rotate(bpl::Rotation::_180);
			Assert::IsTrue(Clone == Expected180, L"(180 degrees)");
			
			Clone = Object;
			Clone.Rotate(bpl::Rotation::_270);
			Assert::IsTrue(Clone == Expected270, L"(270 degrees)");
		}

		TEST_METHOD(EqualityOperators)
		{
			Image<Pixel24> Clone(Object);
			Image<Pixel24> Other(3, 2);

			Assert::IsTrue(Object == Object);
			Assert::IsTrue(Object == Clone);
			Assert::IsFalse(Object == Other);

			Assert::IsFalse(Object != Object);
			Assert::IsFalse(Object != Clone);
			Assert::IsTrue(Object != Other);
		}
	};
}