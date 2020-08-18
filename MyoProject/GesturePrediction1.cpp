#include <iostream>
#include <stdexcept>
#include <vector>
#include <stdlib.h>

#include <myo/myo.hpp>

class PrintMyoEvents : public myo::DeviceListener {
public:
	myo::Pose currentPose;
	bool onArm;
	myo::Arm whichArm;
	bool isUnlocked;
	PrintMyoEvents() : currentPose(), onArm(), whichArm(), isUnlocked() {

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

	std::string CurrentGesture() {
		return currentPose.toString();
	}
};

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

		PrintMyoEvents printer;
		hub.addListener(&printer);

	repeat:;

		std::cout << "!!!!!!!!!!!!!!! Welcome to Myo Passwords !!!!!!!!!!!!!!\n";
		std::cout << "Following are The Numbers Assigned to each Gestue :\n";
		std::cout << "1. Fist\n2. Finger Spread\n3. Wave Out\n4. Wave In\n";
		int random[3];
		random[0] = rand() % 4 + 1;
		random[1] = rand() % 4 + 1;
		random[2] = rand() % 4 + 1;

		std::cout << "You have to Make the Following Code :\n";
		std::cout << random[0] << " " << random[1] << " " << random[2] << "\n\n";

		bool done = false;
		int current = random[0];
		int num = 0;

		while (1) {
			hub.run(500);
			std::string Gesture = printer.CurrentGesture();
			char ch;
			if (Gesture == "unknown" || Gesture == "rest") {
				continue;
			}
			else {
				std::cout << "Your Gesture : " << Gesture << "\n";
				std::cout << "Enter (yes(y)/no(n)) : ";
				std::cin >> ch;
			}
			if (ch != 'y') {
				continue;
			}
			if (Gesture == "fist") {
				if (current == 1) {
					std::cout << "Fist Identified.\n";
					num++;
					if (num == 3) {
						std::cout << "Password Confirmed\n\n";
						goto out;
					}
					current = random[num];
				}
				else {
					std::cout << "Your Login is not authorized\n\n";
					goto out1;
				}
			}
			else if (Gesture == "fingersSpread") {
				if (current == 2) {
					std::cout << "Finger Spread Identified.\n";
					num++;

					if (num == 3) {
						std::cout << "Password Confirmed\n\n";
						goto out;
					}
					current = random[num];
				}
				else {
					std::cout << "Your Login is not authorized\n\n";
					goto out1;
				}
			}
			else if (Gesture == "waveOut") {
				if (current == 3) {
					std::cout << "Wave Out Identified.\n";
					num++;
					if (num == 3) {
						std::cout << "Password Confirmed\n\n";
						goto out;
					}
					current = random[num];
				}
				else {
					std::cout << "Your Login is not authorized\n\n";
					goto out1;
				}
			}
			else if (Gesture == "waveIn") {
				if (current == 4) {
					std::cout << "Wave In Identified.\n";

					num++;
					if (num == 3) {
						std::cout << "Password Confirmed\n\n";
						goto out;
					}
					current = random[num];
				}
				else {
					std::cout << "Your Login is not authorized\n\n";
					goto out1;
				}
			}
		}
	out:;

		std::cout << "You are Logged in....\n";

	out1:;

		int choice = 0;
		std::cout << "Want to login again? (0/1) : ";
		std::cin >> choice;

		if (choice == 1) {
			goto repeat;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << "Press enter to continue.";
		std::cin.ignore();
		return 1;
	}
}*/
