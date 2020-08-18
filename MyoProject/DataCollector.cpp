#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <fstream>
#include <ctype.h>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <utility>
#include <ctime>
#include <chrono>
#include <cstring>

#include <myo/myo.hpp>

std::ofstream file;

class DataCollector : public myo::DeviceListener {
public:
	bool onArm;
	myo::Arm whichArm;
	bool isUnlocked;
	int roll_w, pitch_w, yaw_w;
	myo::Pose currentPose;
	int emgSamples[8];
	std::vector <std::pair<char, double> > acceleration;

	DataCollector()
		: emgSamples(), onArm(), isUnlocked(), roll_w(), pitch_w(), yaw_w(), currentPose()
	{
		onArm = false;
		isUnlocked = false;
		roll_w = 0, pitch_w = 0, yaw_w = 0;
		for (int i = 0; i < 8; i++) {
			emgSamples[i] = 0;
		}
		acceleration.push_back(std::make_pair('x', 0));
		acceleration.push_back(std::make_pair('y', 0));
		acceleration.push_back(std::make_pair('z', 0));
	}

	void onUnpair(myo::Myo* myo, uint64_t timestamp) {
		std::cout << "We have lost the track of Myo\n";
		std::cout << "!!!!!!!!!! Erasing all the data !!!!!!!!!!";
		roll_w = 0;
		pitch_w = 0;
		yaw_w = 0;
		onArm = false;
		isUnlocked = true;
		for (int i = 0; i < 8; i++) {
			emgSamples[i] = 0;
		}
	}

	void onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg) {
		for (int i = 0; i < 8; i++) {
			emgSamples[i] = emg[i];
		}
	}

	void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat) {
		using std::atan2;
		using std::asin;
		using std::sqrt;
		using std::max;
		using std::min;

		float roll = atan2(2.0f * (quat.w() * quat.x() + quat.y() * quat.z()),
			1.0f - 2.0f * (quat.x() * quat.x() + quat.y() * quat.y()));
		float pitch = asin(max(-1.0f, min(1.0f, 2.0f * (quat.w() * quat.y() - quat.z() * quat.x()))));
		float yaw = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
			1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));

		roll_w = static_cast<int>((roll + (float)M_PI) / (M_PI * 2.0f) * 18);
		pitch_w = static_cast<int>((pitch + (float)M_PI / 2.0f) / M_PI * 18);
		yaw_w = static_cast<int>((yaw + (float)M_PI) / (M_PI * 2.0f) * 18);
	}

	void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose) {
		currentPose = pose;

		if (pose != myo::Pose::unknown && pose != myo::Pose::rest) {
			myo->unlock(myo::Myo::unlockHold);
			myo->notifyUserAction();
		}
		else {
			myo->unlock(myo::Myo::unlockHold);
		}
	}

	void onArmSync(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection, float rotation, myo::WarmupState warmupState) {
		onArm = true;
		whichArm = arm;
	}

	void onArmUnsync(myo::Myo* myo, uint64_t timestamp) {
		onArm = false;
	}

	void onUnlock(myo::Myo* myo, uint64_t timestamp) {
		isUnlocked = true;
	}

	void onLock(myo::Myo* myo, uint64_t timestamp) {
		isUnlocked = false;
		onUnlock(myo, timestamp);
	}

	void onAccelerometerData(myo::Myo* myo, uint64_t timestamp, const myo::Vector3<float>& input_acceleration) {
		acceleration[0] = std::make_pair('x', input_acceleration[0]);
		acceleration[1] = std::make_pair('y', input_acceleration[1]);
		acceleration[2] = std::make_pair('z', input_acceleration[2]);
	}

	void print(std::string filename, int count) {
		std::string data[15];

		std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch());
		//data[0] = std::to_string((long long int)ms.count());
		data[0] = std::to_string (count);
		for (int i = 0; i < data[0].length(); i++) {
			if (data[0][i] == '\n') {
				data[0][i] = '\0';
				break;
			}
		}

		for (int i = 0; i < 8; i++) {
			data[i + 1] = std::to_string(emgSamples[i]);
		}

		data[9] = std::to_string(acceleration[0].second);
		data[10] = std::to_string(acceleration[1].second);
		data[11] = std::to_string(acceleration[2].second);

		data[12] = std::to_string(roll_w);
		data[13] = std::to_string(pitch_w);
		data[14] = std::to_string(yaw_w);

		for (int i = 0; i < 15; i++) {
			file << data[i] << ",";
		}
		file << std::endl;
	}
};

bool fileExists(const std::string& filename)
{
	struct stat buf;
	if (stat(filename.c_str(), &buf) != -1)
	{
		return true;
	}
	return false;
}

/*int main(int argc, char** argv)
{
	try {
		myo::Hub hub("com.example.hello-myo");

		std::cout << "Attempting to find a Myo..." << std::endl;
		myo::Myo* myo = hub.waitForMyo(10000);

		if (!myo) {
			throw std::runtime_error("Unable to find a Myo!");
		}

		std::cout << "Connected to a Myo armband!" << std::endl << std::endl;
		DataCollector collector;

		myo->setStreamEmg(myo::Myo::streamEmgEnabled);

	repeat:;

		hub.addListener(&collector);
		std::string gesture;
		std::cout << "Enter the Gesture which you are going to perform : ";
		std::cin >> gesture;

		int count = 0;
		std::cout << "Enter the number of times you are going to perform the gesture : ";
		std::cin >> count;

		while (count--) {
			char ch;
			std::cout << "Click s to start the Recording\n";
			std::cin >> ch;
			while (ch != 's') {
				std::cin >> ch;
			}

			int num = 1;
			std::string filename = gesture + '_';
			filename += std::to_string(num);
			filename += ".csv";

			while (fileExists(filename)) {
				num++;
				filename = gesture + '_' + std::to_string(num) + ".csv";
			}

			filename = gesture + '_' + std::to_string(num) + ".csv";
			std::cout << "Your File name is : " << filename << "\n";

			file.open(filename);
			//file << "Time Stamp, EMG_1, EMG_2, EMG_3, EMG_4, EMG_5, EMG_6, EMG_7, EMG_8, AccelerationX, AccelerationY, AccelerationZ, Orientation_1, Orientation_2, Orientation_3" << std::endl;

			int time = 1;

			while (time <= 1000) {
				hub.run(8);
				collector.print(filename, time);
				//if (GetAsyncKeyState(VK_ESCAPE)) {
				//	break;
				//}
				time++;
				std::cout << time << "\n";
			}
			file.close();
		}

		int choice;
		std::cout << "Want to collect more data(no->0/yes->1)\n?";
		std::cin >> choice;
		if (choice == 1) {
			goto repeat;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << "Press enter to continue.";
		std::cin.ignore();
		return 0;
	}
	return 0;
}*/