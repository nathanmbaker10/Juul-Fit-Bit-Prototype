//
//  ViewController.swift
//  Juul_IOS
//
//  Created by Andre Assadi on 6/26/20.
//  Copyright © 2020 AndreAssadiProjects. All rights reserved.
//

import UIKit
import FirebaseFirestore


class ViewController: UIViewController {
    @IBOutlet weak var cartridgeStatusLabel: UILabel!
    @IBOutlet weak var doubleTapCountLabel: UILabel!
    @IBOutlet weak var hitCountLabel: UILabel!
    static let db = Firestore.firestore()
    let documentReference = ViewController.db.collection("users").document("u1")
    
    override func viewDidLoad() {
        super.viewDidLoad()
        populateInitialLables()
        addDataChangeListeners()
        // Do any additional setup after loading the view.
    }
    
    func populateInitialLables() {
        self.documentReference.getDocument { (documentSnapshot, error) in
            if let document = documentSnapshot, document.exists {
//                print("Document data: \(dataDescription)")
                self.changeLabelText(document: document)
            } else {
                print("Document does not exist")
            }
        }
    }
    
    func addDataChangeListeners() {
        self.documentReference.addSnapshotListener { documentSnapshot, error in
          guard let document = documentSnapshot else {
            print("Error fetching document: \(error!)")
            return
            }
            self.changeLabelText(document: document)
        }

    }
    
    func changeLabelText(document: DocumentSnapshot) {
        if let data = document.data() {
            print(data)
            if data["cartridge-in"] as! Bool {
                self.cartridgeStatusLabel.text = "Cartridge Status: In"
            } else {
                self.cartridgeStatusLabel.text = "Cartridge Status: Out"
            }
            
            self.doubleTapCountLabel.text = "Double Taps: \(data["double-taps"]!)"
            self.hitCountLabel.text = "Hits: \(data["hits"]!)"
        }
    }


}

