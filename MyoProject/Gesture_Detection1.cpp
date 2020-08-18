#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include <vector>
#include <utility>

#include <myo/myo.hpp>

class PoseDetection : public myo::DeviceListener {
public:
	myo::Pose currentPose;
	bool onArm;
	myo::Arm whichArm;
	bool isUnlocked;
	std::vector<int> password;
	int password_length = 0;
	PoseDetection() : currentPose(), onArm(), whichArm(), isUnlocked(), password_length(0) {

	}

	void setPassword(std::vector<int> password,int n) {
		password_length = n;
		for (int i = 0; i < n; i++) {
			(this->password).push_back(password[i]);
		}
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

	bool verify(std::vector <int> password) {
		for (int i = 0; i < password_length; i++) {
			if (this->password[i] != password[i]) {
				return false;
			}
		}
		return true;
	}

	void printPassword() {
		for (int i = 0; i < password_length; i++) {
			std::cout << password[i] << " ";
		}
		std::cout << "\n";
	}
};

int main() {
	try {
		myo::Hub hub("com.example.hello-myo");

		std::cout << "Attempting to find a Myo..." << std::endl;
		myo::Myo* myo = hub.waitForMyo(10000);

		if (!myo) {
			throw std::runtime_error("Unable to find a Myo!");
		}
		std::cout << "Connected to a Myo armband!" << std::endl << std::endl;
	
	repeat:;

		PoseDetection pose;
		hub.addListener(&pose);

		std::cout << "!!!!!!!!!!!!!!! Gesture Prediction - 2 !!!!!!!!!!!!!!!\n";
		std::cout << "Following are The Numbers Assigned to each Gestue :\n";
		std::cout << "1. Fist\n2. Finger Spread\n3. Wave Out\n4. Wave In\n";
		/*std::vector <int> random(3);
		random[0] = rand() % 4 + 1;
		random[1] = rand() % 4 + 1;
		random[2] = rand() % 4 + 1;

		std::cout << "Password to Perform : " << random[0] << " " << random[1] << " " << random[2] << "\n";


		pose.setPassword(random, 3);*/

        std::cout << "Press Enter to Set Your Password\n";
        std::cin.ignore()
        std::vector <int> pass;
        std::string currentGesture = "";
		std::string lastGestureRecorded = "";
		int count = 0, lastCount = 0;
		int temp = 1;

        while (pass.size() != 3) {
            hub.run (2)
            std::string Gesture = pose.CurrentGesture();

            if (Gesture == "unknown" || Gesture == "rest") {                              // To Neglect all the unknown and rest poses
				continue;
			}

            if (temp != 1) {
				if (lastGestureRecorded == Gesture && lastCount != 100) {
					lastCount++;
					continue;
				}
				else if (lastGestureRecorded == Gesture && lastCount == 100) {
					lastCount = 0;
					lastGestureRecorded = "unknown";
				}
			}

			if (currentGesture == Gesture) {											  // If the Current Gesture is same as the Last Gesture
				count++;
			}
			else {                                                                        // If a new Gesture is Performed
				count = 0;                                                                // Counting again from 0
				currentGesture = Gesture;
				continue;
			}

			if (count == 400) {                                                           // If the current Gesture is counted 200 times
				std::cout << "Gesture Performed : " << Gesture << "\n";
				if (Gesture == "fist") {                                                  // Gesture Performed is Fist
					pass_performed.push_back(1);                                          // Pushing the Id of Gesture performed in the vector pass_performed
					lastGestureRecorded = "fist";
				}
				else if (Gesture == "fingersSpread") {                                    // Gesture Performed is Finger Spread
					pass_performed.push_back(2);                                          // Push
					lastGestureRecorded = "fingersSpread";
				}
				else if (Gesture == "waveOut") {
					pass_performed.push_back(3);
					lastGestureRecorded = "waveOut";
				}
				else if (Gesture == "waveIn") {
					pass_performed.push_back(4);
					lastGestureRecorded = "waveIn";
				}
				count = 0;
				lastCount = 0;
				temp++;
			}

			if (temp == 4) {
				break;
			}
        }

		std::cout << "Press Enter to login\n";
		std::cin.ignore();

		std::vector <int> pass_performed;
		currentGesture = "";
		lastGestureRecorded = "";
		count = 0
        lastCount = 0;
		temp = 1;
		while (1) {
			hub.run(2);                                                                   // Frequency of 200 Hz.
			std::string Gesture = pose.CurrentGesture();                                  // To Get the Gesture from the Class Pose Detection

			if (Gesture == "unknown" || Gesture == "rest") {                              // To Neglect all the unknown and rest poses
				continue;
			}

			if (temp != 1) {
				if (lastGestureRecorded == Gesture && lastCount != 100) {
					lastCount++;
					continue;
				}
				else if (lastGestureRecorded == Gesture && lastCount == 100) {
					lastCount = 0;
					lastGestureRecorded = "unknown";
				}
			}

			if (currentGesture == Gesture) {											  // If the Current Gesture is same as the Last Gesture
				count++;
			}
			else {                                                                        // If a new Gesture is Performed
				count = 0;                                                                // Counting again from 0
				currentGesture = Gesture;
				continue;
			}

			if (count == 400) {                                                           // If the current Gesture is counted 200 times
				std::cout << "Gesture Performed : " << Gesture << "\n";
				if (Gesture == "fist") {                                                  // Gesture Performed is Fist
					pass_performed.push_back(1);                                          // Pushing the Id of Gesture performed in the vector pass_performed
					lastGestureRecorded = "fist";
				}
				else if (Gesture == "fingersSpread") {                                    // Gesture Performed is Finger Spread
					pass_performed.push_back(2);                                          // Push
					lastGestureRecorded = "fingersSpread";
				}
				else if (Gesture == "waveOut") {
					pass_performed.push_back(3);
					lastGestureRecorded = "waveOut";
				}
				else if (Gesture == "waveIn") {
					pass_performed.push_back(4);
					lastGestureRecorded = "waveIn";
				}
				count = 0;
				lastCount = 0;
				temp++;
			}

			if (temp == 4) {
				break;
			}
		}

		bool login = pose.verify(pass_performed);

		if (login) {
			std::cout << "Your Login was Successful\n";
		}
		else {
			std::cout << "Your Login was Unsuccessful\n";
			std::cout << "Original Password : "; 
			pose.printPassword();
			std::cout << "Password Performed : " << pass_performed[0] << " " << pass_performed[1] << " " << pass_performed[2] << "\n";
		}
		
		char choice;
		std::cout << "Do you want to perform the Login again (y/n) ?\n";
		std::cin >> choice;
		if (choice == 'y' || choice == 'Y') {
			goto repeat;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << "Press enter to continue.";
		std::cin.ignore();
		return 1;
	}
	return 0;
}