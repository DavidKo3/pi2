
#include "io/sequence.h"
#include "io/alphanum.h"
#include "io/raw.h"
#include "pointprocess.h"
#include <iostream>
#include <algorithm>

using namespace std;
using namespace math;

namespace itl2
{
	namespace sequence
	{
		namespace internals
		{
			/**
			Tests if given str matches template.
			In the template
			* matches to sequence of any characters
			? matches to any single character
			@ matches to one or more numerical digits
			*/
			bool matches(const string& str, const string& templ)
			{
				size_t strPos = 0;
				size_t templPos = 0;
				while (true)
				{
					if (strPos >= str.length())
						return templPos == templ.length();

					if (templPos >= templ.length())
						return false;

					if (templ[templPos] == '?')
					{
						// Any character is ok, no need to test.
						templPos++;
						strPos++;
					}
					else if (templ[templPos] == '*')
					{
						if (templPos >= templ.length() - 1)
							return true;

						for (size_t n = strPos; n < str.length(); n++)
							if (matches(str.substr(n), templ.substr(templPos + 1)))
								return true;

						return false;
					}
					else if (templ[templPos] == '@')
					{
						if (!isdigit(str[strPos]))
							return false;

						while(strPos < str.length() && isdigit(str[strPos]))
							strPos++;

						templPos++;
					}
					else
					{
						// Characters must be equal.
						if (str[strPos] != templ[templPos])
							return false;
						strPos++;
						templPos++;
					}
				}
			}

			/**
			Separates directory and filename parts of a sequence template.
			*/
			void separatePathAndFileTemplate(const string& templ, fs::path& dir, string& fileTemplate)
			{
				fs::path p(templ);

				if (fs::is_directory(p))
				{
					dir = p;
					fileTemplate = "";
				}
				else
				{
					dir = p.parent_path();
					fileTemplate = p.filename().string();
					if (fileTemplate == ".")
						fileTemplate = "";
				}
			}

			///*
			//Separates directory and filename parts of a sequence template.
			//*/
			//void separatePathAndFileTemplate(const string& templ, fs::path& dir, string& fileTemplate)
			//{
			//	fs::path p;
			//	separatePathAndFileTemplate(templ, p, fileTemplate);
			//	dir = p.string();
			//}

			
			vector<string> buildFileList(const string& templ)
			{
				// Separate directory and file name template
				string fileTemplate;
				fs::path dir;
				separatePathAndFileTemplate(templ, dir, fileTemplate);

				if(fileTemplate == "")
					fileTemplate = "*";
				
				//cout << "Directory: " << dir << endl;
				//cout << "Template: " << fileTemplate << endl;

				// Get those files in directory that match the template
				vector<string> filenames;
				for (auto & p : fs::directory_iterator(dir))
				{
					string filename = p.path().filename().string();
					if (matches(filename, fileTemplate))
						filenames.push_back(p.path().string());
				}
				
				// Sort to natural order
				sort(filenames.begin(), filenames.end(), doj::alphanum_less<std::string>());

				//for (size_t n = 0; n < filenames.size(); n++)
				//	cout << filenames[n] << endl;
				return filenames;
			}

			
		}

		namespace tests
		{
			void matchTest(const string& str, const string& templ, bool expected)
			{
				bool result = internals::matches(str, templ);
				cout << str << " / " << templ << " = " << result;
				if (result == expected)
					cout << endl;
				else
					cout << " FAIL" << endl;
				testAssert(result == expected, "matching");
			}

			void match()
			{
				matchTest("abc1", "abc@", true);
				matchTest("abc10", "abc@", true);
				matchTest("abc1x", "abc@", false);
				matchTest("abc1x", "abc@x", true);
				matchTest("abc11x", "abc@x", true);

				matchTest("test_sequence0000.png", "test_sequence@.png", true);
				matchTest("test_sequence_2_0014.png", "test_sequence@.png", false);
				matchTest("test_sequence2_0014.png", "test_sequence@.png", false);

				matchTest("abc", "*", true);
				matchTest("abc", "a*", true);
				matchTest("abc", "b*", false);
				matchTest("abc", "*b", false);
				matchTest("abc", "*bc", true);
				matchTest("abc", "*b*", true);
				matchTest("abc", "***", true);
				matchTest("abc", "*ab", false);

				matchTest("abc", "", false);
				matchTest("abc", "a", false);
				matchTest("abc", "ab", false);
				matchTest("abc", "abc", true);
				matchTest("abc", "a?b", false);
				matchTest("abc", "a?c", true);
				matchTest("abc", "ab?", true);
				matchTest("abc", "?bc", true);
				matchTest("abc", "abcd", false);

				matchTest("filename_001.png", "filename_*.png", true);
				matchTest("filename_002.png", "filename_*.png", true);
				matchTest("filename2_002.png", "filename_*.png", false);
				matchTest("filename_002.tif", "filename_*.png", false);
				matchTest("filename_001.png", "*.png", true);
			}

			void buildFileList()
			{
				internals::buildFileList("C:\\mytemp\\dev\\itl2\\testing\\sequence\\test_seq\\test_sequence@.png");
				internals::buildFileList("C:\\mytemp\\dev\\itl2\\testing\\sequence\\test_seq\\");
				internals::buildFileList("C:\\mytemp\\dev\\itl2\\testing\\sequence\\test_seq");
			}

			void sequence()
			{
				// NOTE: No asserts!

				//Image<uint8_t> img;
				//sequence::read(img, "./sequence/test_seq/test_sequence@.png");
				//raw::writed(img, "./sequence/test_sequence");

				Image<uint8_t> img2;
				sequence::read(img2, "./test_seq/t1-head_bin@.png");
				raw::writed(img2, "./sequence/test_sequence");

				sequence::write(img2, "./sequence/save_test/auto_@(-).png");
				sequence::write(img2, "./sequence/save_test/zero_@.png");
				sequence::write(img2, "./sequence/save_test/ten_@(10).png");
				sequence::write(img2, "./sequence/save_malformed/two_@(2");
				sequence::write(img2, "./sequence/save_to_folder_test/");
				sequence::write(img2, "./sequence/save_to_folder_test2");

				sequence::write(img2, "sequence_save_test");
				sequence::read(img2, "sequence_save_test");
				sequence::read(img2, "./sequence/save_to_folder_test2");

				// Partial read and write
				sequence::read(img2, "./test_seq/t1-head_bin@.png", 100, 110);
				sequence::write(img2, "./sequence/partial_100-110/", 100);

			}

			void readWriteBlock()
			{
				// NOTE: No asserts!

				Image<uint16_t> head(256, 256, 129);
				raw::read(head, "./t1-head_256x256x129.raw");
				sequence::write(head, "./sequence/readwriteblock/write_normal/");

				Image<uint16_t> block(100, 100, 100);
				

				Vec3c outputDimensions = round(1.5 * Vec3d(head.dimensions()));
				string outFile = "./sequence/readwriteblock/head_3D_montage";

				Vec3c blockStart(50, 50, 0);
				Vec3c blockSize = head.dimensions() - blockStart - Vec3c(0, 10, 0);

				for (coord_t z = 0; z < 2; z++)
				{
					for (coord_t y = 0; y < 2; y++)
					{
						for (coord_t x = 0; x < 2; x++)
						{
							cout << x << "/2, " << y << "/2, " << z << "/2" << endl;
							Vec3c pos(x * blockSize.x, y * blockSize.y, z * blockSize.z);
							sequence::writeBlock(head, outFile, pos, outputDimensions, blockStart, blockSize, true);
						}
					}
				}

			}
		}
	}
}