pragma solidity ^0.5.0;
contract p5 {
    string public name;
    uint public productCount = 0;
    
    mapping(uint => Product) public products;
    
    struct Product {
        uint id;
        string name;
        uint price;
        address payable owner;
        bool purchased;
    }

    event ProductCreated(
        uint id,
        string name,
        uint price,
        address payable owner,
        bool purchased
    );

    event ProductPurchased(
        uint id,
        string name,
        uint price,
        address payable owner,
        bool purchased
    );

    constructor() public {
        name = "Dapp University Marketplace";
    }

    function createProduct(string memory _name, uint _price) public {
        // Require a valid name
        require(bytes(_name).length > 0);
        // Require a valid price
        require(_price > 0);
        // Increment product count
        productCount ++;
        // Create the product
        products[productCount] = Product(productCount, _name, _price, msg.sender, false);
        // Trigger an event
        emit ProductCreated(productCount, _name, _price, msg.sender, false);
    }
    
    function purchaseProduct(uint _id) public payable {
        // Fetch the product
        Product memory _product = products[_id];
        // Fetch the owner
        address payable _seller = _product.owner;
        // Make sure the product has a valid id
        require(_product.id > 0 && _product.id <= productCount);
        // Require that there is enough Ether in the transaction
        require(msg.value >= _product.price);
        // Require that the product has not been purchased already
        require(!_product.purchased);
        // Require that the buyer is not the seller
        require(_seller != msg.sender);
        // Transfer ownership to the buyer
        _product.owner = msg.sender;
        // Mark as purchased
        _product.purchased = true;
        // Update the product
        products[_id] = _product;
        // Pay the seller by sending them Ether
        address(_seller).transfer(msg.value);
        // Trigger an event
        emit ProductPurchased(productCount, _product.name, _product.price, msg.sender, true);
    }
}




pragma solidity >=0.4.21 <0.9.0;

contract supplyChain {
    uint32 public product_id = 0; // Product ID
    uint32 public participant_id = 0; // Participant ID
    uint32 public owner_id = 0; // Ownership ID
    
    struct product {
        string modelNumber;
        string partNumber;
        string serialNumber;
        address productOwner;
        uint32 cost;
        uint32 mfgTimeStamp;
    }

    mapping(uint32 => product) public products;
    
    struct participant {
        string userName;
        string password;
        string participantType;
        address participantAddress;
    }

    mapping(uint32 => participant) public participants;

    struct ownership {
        uint32 productId;
        uint32 ownerId;
        uint32 trxTimeStamp;
        address productOwner;
    }

    mapping(uint32 => ownership) public ownerships; // ownerships by ownership ID (owner_id)

    mapping(uint32 => uint32[]) public productTrack; // ownerships by Product ID (product_id) / Movement 

    //track for a product
    event TransferOwnership(uint32 productId);

    function addParticipant(string memory _name, string memory _pass, address _pAdd, string memory _pType) public returns (uint32){
        uint32 userId = participant_id++;
        participants[userId].userName = _name;
        participants[userId].password = _pass;
        participants[userId].participantAddress = _pAdd;
        participants[userId].participantType = _pType;
        return userId;
    }

    function getParticipant(uint32 _participant_id) public view returns (string memory,address,string memory) {
        return (participants[_participant_id].userName,
        participants[_participant_id].participantAddress,
        participants[_participant_id].participantType);
    }

    function addProduct(uint32 _ownerId,string memory _modelNumber,string memory _partNumber,string
    memory _serialNumber,uint32 _productCost) public returns (uint32) {
        if(keccak256(abi.encodePacked(participants[_ownerId].participantType)) == keccak256("Manufacturer")) {
        uint32 productId = product_id++;
        products[productId].modelNumber = _modelNumber;
        products[productId].partNumber = _partNumber;
        products[productId].serialNumber = _serialNumber;
        products[productId].cost = _productCost;
        products[productId].productOwner = participants[_ownerId].participantAddress;
        products[productId].mfgTimeStamp = uint32(block.timestamp);
        return productId;
        }
        return 0;
    }
    modifier onlyOwner(uint32 _productId) {
        require(msg.sender == products[_productId].productOwner,"");
        _;
    }
    function getProduct(uint32 _productId) public view returns (string memory,string memory,string
    memory,uint32,address,uint32){
        return (products[_productId].modelNumber,
        products[_productId].partNumber,
        products[_productId].serialNumber,
        products[_productId].cost,
        products[_productId].productOwner,
        products[_productId].mfgTimeStamp);
    }
    function newOwner(uint32 _user1Id,uint32 _user2Id, uint32 _prodId) onlyOwner(_prodId) public returns (bool) {

        participant memory p1 = participants[_user1Id];
        participant memory p2 = participants[_user2Id];
        uint32 ownership_id = owner_id++;
        
        if(keccak256(abi.encodePacked(p1.participantType)) == keccak256("Manufacturer")&& 
        keccak256(abi.encodePacked(p2.participantType))==keccak256("Supplier")){
            ownerships[ownership_id].productId = _prodId;
            ownerships[ownership_id].productOwner = p2.participantAddress;
            ownerships[ownership_id].ownerId = _user2Id;
            ownerships[ownership_id].trxTimeStamp = uint32(block.timestamp);
            products[_prodId].productOwner = p2.participantAddress;
            productTrack[_prodId].push(ownership_id);
            emit TransferOwnership(_prodId);
            return (true);
        }
        else if(keccak256(abi.encodePacked(p1.participantType)) == keccak256("Supplier") &&
        keccak256(abi.encodePacked(p2.participantType))==keccak256("Supplier")){
            ownerships[ownership_id].productId = _prodId;
            ownerships[ownership_id].productOwner = p2.participantAddress;
            ownerships[ownership_id].ownerId = _user2Id;
            ownerships[ownership_id].trxTimeStamp = uint32(block.timestamp);
            products[_prodId].productOwner = p2.participantAddress;
            productTrack[_prodId].push(ownership_id);
            emit TransferOwnership(_prodId);
            return (true); 
        }
        else if(keccak256(abi.encodePacked(p1.participantType)) == keccak256("Supplier") &&
        keccak256(abi.encodePacked(p2.participantType))==keccak256("Consumer")){
            ownerships[ownership_id].productId = _prodId;
            ownerships[ownership_id].productOwner = p2.participantAddress;
            ownerships[ownership_id].ownerId = _user2Id;
            ownerships[ownership_id].trxTimeStamp = uint32(block.timestamp);
            products[_prodId].productOwner = p2.participantAddress;
            productTrack[_prodId].push(ownership_id);
            emit TransferOwnership(_prodId);
            return (true);
        }
        return (false);
    }

    function getProvenance(uint32 _prodId) external view returns (uint32[] memory) {
        return productTrack[_prodId];
    }

    function getOwnership(uint32 _regId) public view returns (uint32,uint32,address,uint32) {
        ownership memory r = ownerships[_regId];
        return (r.productId,r.ownerId,r.productOwner,r.trxTimeStamp);
    }
    
    function authenticateParticipant(uint32 _uid,string memory _uname,string memory _pass,string memory
    _utype) public view returns (bool){
        if(keccak256(abi.encodePacked(participants[_uid].participantType)) == 
        keccak256(abi.encodePacked(_utype))) {
            if(keccak256(abi.encodePacked(participants[_uid].userName)) == 
            keccak256(abi.encodePacked(_uname))) {
                if(keccak256(abi.encodePacked(participants[_uid].password)) == 
                keccak256(abi.encodePacked(_pass))) {
                return (true);
                }
            }
        }
        return (false);
    }
}




genesis.json
{ "config": { "chainId": 15, "homesteadBlock": 0, "eip155Block": 0, "eip158Block": 0 }, "difficulty": "0x400", "gasLimit": "0x2100000", "alloc": { "7a69b359e86893efa3d9732e4c65ced51567edd0": { "balance": "0x1337000000000000000000" } } }
geth –datadir node1 init genesis.json
geth — datadir node1/ — syncmode “full” — port 30304 — http — http.addr “localhost” — http.port 8545 — http.api “personal,eth,net,web3,txpool,miner” — networkid 99 — identity node1 console
Personal.newAccount()
start.miner(1)
eth.getBalance(eth.accounts[0])
admin.nodeInfo.enode -> return enode ids
admin.addPeer(“enode://”)
admin.peers



pragma solidity ^0.8.10;

// contract p4{
//     uint public v1 = 10;    // 0 -> byDefault
//     int public v2 = 20;     // 0 -> byDefault  
//     bool public v3 = false; //false -> byDefault

//     uint public v4;

//     // function f1() public {
//     //     int s1 = 123;
//     // }

//     function set(uint _v4) public {
//         v4 = _v4;
//     }

//     function get() public view returns(uint) {
//         return v4;
//     }

//     function f2(uint x) public returns(uint) {
//         if(x<5){
//             return 0;
//         }else if(x<10){
//             return 1;
//         }else{
//             return 2;
//         }
//     }

//     function loop1() public {
//         for(uint i=0;i<10;i++){
//             if(i == 3){
//                 continue;
//             }
//             if(i == 7){
//                 break;
//             }
//         }
//     }

// }

contract A{
    uint private a;

    function f3(uint x, uint z) internal returns(uint){
        return x+z;
    }

    function f4(uint x) private returns(uint b){
        return 4+x;
    }
}

contract B{
    function f5() public{
        A ctA = new A();
        //uint var = ctA.f2(80); //compillation error
    }
}

contract C is A {
    function f6() public {
        A contA = new A();
        uint var2 = f3(74, 51); 
    } 
}


