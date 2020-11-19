using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MainCameraEvents : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    /*
     void Update()
    {
        keepPlayerInCenter();
    }
    
    private void keepPlayerInCenter()
    {
        Rigidbody2D player = GameObject.Find("Player").GetComponent<Rigidbody2D>();
        Rect screenBounds = new Rect(118, 100, 188, 150);
        if (player.position.x < screenBounds.x)
        {
            moveCamera(-1, 0);
        }
        else if (player.position.x > screenBounds.xMax)
        {
            moveCamera(1, 0);
        }

        if (player.position.y < screenBounds.y)
        {
            moveCamera(0, -1);
        }
        else if (player.position.y > screenBounds.yMax)
        {
            moveCamera(0, 1);
        }


        Debug.Log("player.position:" + player.transform.localPosition.x + "/" + player.transform.localPosition.y);
    }

    private void moveCamera(int x, int y)
    {
        Transform mainCameraTransform = GameObject.Find("Main Camera").GetComponent<Camera>().GetComponent<Transform>();
        Vector3 endPoint = new Vector3(transform.position.x + (x * 40), transform.position.y + (y * 40), 0);
        transform.position = Vector3.Lerp(transform.position, endPoint, .5f);

    }
    */

}
