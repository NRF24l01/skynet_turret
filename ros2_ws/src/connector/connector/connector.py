import rclpy
from rclpy.node import Node
from turret_msg.msg import Control
import serial

class ControlSerialNode(Node):
    def __init__(self):
        super().__init__('control_serial_node')

        # Параметры
        self.declare_parameter('port', '/dev/ttyACM0')
        self.declare_parameter('baudrate', 115200)

        port = self.get_parameter('port').get_parameter_value().string_value
        baud = self.get_parameter('baudrate').get_parameter_value().integer_value

        # Serial
        try:
            self.serial = serial.Serial(port, baudrate=baud, timeout=1)
            self.get_logger().info(f'Serial открыт: {port} @ {baud}')
        except serial.SerialException as e:
            self.get_logger().error(f'Ошибка открытия serial: {e}')
            raise e

        # Подписка
        self.subscription = self.create_subscription(
            Control,
            '/control',
            self.control_callback,
            10
        )

    def control_callback(self, msg: Control):
        try:
            fire_str = '1' if msg.fire else '0'
            data_str = f"{msg.speed+90}|{msg.angle}|{fire_str}\n"
            self.serial.write(data_str.encode('utf-8'))
            self.get_logger().info(f'Отправлено: {data_str.strip()}')
        except Exception as e:
            self.get_logger().error(f'Ошибка отправки в serial: {e}')

    def destroy_node(self):
        if hasattr(self, 'serial') and self.serial.is_open:
            self.serial.close()
        super().destroy_node()

def main(args=None):
    rclpy.init(args=args)
    node = ControlSerialNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()
